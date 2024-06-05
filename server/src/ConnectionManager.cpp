#include "ConnectionManager.h"
#include "Asset.h"
#include "GameManager.h"
#include "LevelManager.h"
#include "Player.h"
#include "PlayerManager.h"

#pragma warning(push)
#pragma warning(disable : 4267)
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning(pop)

#include <nlohmann/json.hpp>

#include <functional>
#include <map>
#include <mutex>
#include <string>

using namespace Network;
using namespace Gameplay;

using json = nlohmann::json;
using namespace nlohmann::literals;

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::connection<websocketpp::config::asio> connection;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

typedef server::message_ptr message_ptr;

std::unique_ptr<Impl> ConnectionManager::m_impl = nullptr;
std::vector<int> ConnectionManager::m_assetsToRemove;
std::mutex connectionMutex;
std::mutex removalMutex;

class Network::Impl
{
  public:
    server endpoint;
    std::map<std::shared_ptr<connection>, std::shared_ptr<Gameplay::Player>> connections;

    Impl()
    {
        thread = std::thread(&websocketThread, this);
    }

    void Send(std::shared_ptr<connection> connection, std::string message)
    {
        if (connection->get_state() == websocketpp::session::state::open)
        {
            printf("Data sent\n");
            endpoint.send(connection, message.c_str(), websocketpp::frame::opcode::text);
        }
    }

  private:
    std::thread thread;

    static void websocketThread(Impl *impl)
    {
        auto &endpoint = impl->endpoint;

        // endpoint.set_access_channels(websocketpp::log::alevel::all);
        // endpoint.clear_access_channels(websocketpp::log::alevel::frame_payload);
        endpoint.clear_access_channels(websocketpp::log::alevel::all);
        endpoint.set_access_channels(websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect);
        endpoint.init_asio();

        endpoint.set_close_handler(bind(&Impl::on_close, &endpoint, ::_1));
        endpoint.set_message_handler(bind(&Impl::on_message, &endpoint, ::_1, ::_2));

        endpoint.listen(9002);
        endpoint.start_accept();

        endpoint.run();
    }

    static void on_close(server *s, websocketpp::connection_hdl hdl)
    {
        ConnectionManager::m_impl->CloseConnection(s->get_con_from_hdl(hdl));
    }

    static void on_message(server *s, websocketpp::connection_hdl hdl, message_ptr msg)
    {
        try
        {
            json j = json::parse(msg->get_payload());

            if (j.contains("type"))
            {
                if (j["type"] == "conreq")
                {
                    s->send(hdl, ConnectionManager::CreateStatusMessage("Pending connection"),
                            websocketpp::frame::opcode::text);

                    ConnectionManager::m_impl->CompleteConnection(s->get_con_from_hdl(hdl));
                }
                if (j["type"] == "statusreq")
                {
                    int numConnection = 0;
                    connectionMutex.lock();
                    numConnection = ConnectionManager::m_impl->connections.size();
                    connectionMutex.unlock();

                    s->send(hdl, ConnectionManager::CreateStatusMessage(numConnection > 0 ? "good" : "empty"),
                            websocketpp::frame::opcode::text);
                }
                else if (j["type"] == "input")
                {
                    ConnectionManager::m_impl->SetInput(s->get_con_from_hdl(hdl), j);
                }
            }
            else
            {
                s->send(hdl, ConnectionManager::CreateErrorMessage("Unknown request"),
                        websocketpp::frame::opcode::text);
            }
        }
        catch (json::parse_error &e)
        {
            std::cerr << "Parse error: " << e.what() << std::endl;
        }
        catch (std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void CloseConnection(const std::shared_ptr<connection> &handle)
    {
        connectionMutex.lock();
        auto it = connections.find(endpoint.get_con_from_hdl(handle));
        if (it != connections.end() && it->second != nullptr)
        {
            it->second->m_pendingRemove = true;
            connections.erase(it);
        }
        connectionMutex.unlock();
    }

    void CompleteConnection(const std::shared_ptr<connection> &handle)
    {
        connectionMutex.lock();
        connections.insert({handle, nullptr});

        endpoint.send(handle, ConnectionManager::CreateStatusMessage("Connection established"),
                      websocketpp::frame::opcode::text);
        connectionMutex.unlock();
    }

    void SetInput(const std::shared_ptr<connection> &handle, const json &j)
    {
        connectionMutex.lock();
        auto it = connections.find(endpoint.get_con_from_hdl(handle));
        if (it != connections.end() && it->second != nullptr)
        {
            auto input = j["input"];
            it->second->SetInputs(input["sledge"], input["move"], input["jump"]);
        }
        connectionMutex.unlock();
    }
};

ConnectionManager::ConnectionManager(PlayerManager *playerManager, Gameplay::LevelManager *levelManager)
    : m_playerManager(playerManager), m_levelManager(levelManager), m_tickCounter(0.0f), m_cachedScore(0)
{
    m_impl = std::make_unique<Impl>();
}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::Update(float deltaTime)
{
    bool tick = false;
    m_tickCounter += deltaTime;
    if (m_tickCounter > 1.0f / 60.0f)
    {
        tick = true;
        m_tickCounter = 0.0f;
    }

    connectionMutex.lock();
    bool playerJoined = false;
    for (auto &connection : m_impl->connections)
    {
        if (connection.second == nullptr && connection.first->get_state() == websocketpp::session::state::open)
        {
            auto player = m_playerManager->CreatePlayer();
            connection.second = player;
            playerJoined = true;

            if (m_playerManager->GetNumPlayers() <= 2)
            {
                m_playerManager->ClearScore();
                m_levelManager->NextLevel(GameModeType::Brawl);
                m_cachedScore = 0;
            }
        }
    }

    if (tick == true || playerJoined)
    {
        printf("Sending data\n");

        auto assets = m_levelManager->GetAssets(playerJoined);
        auto playerAssets = m_playerManager->GetAssets();

        assets.insert(assets.end(), playerAssets.begin(), playerAssets.end());
        SendAssets(assets, playerJoined);

        if (playerJoined == false || m_playerManager->GetScore() != m_cachedScore)
        {
            SendScore();
            m_cachedScore = m_playerManager->GetScore();
        }

        RemoveAssets();
    }

    connectionMutex.unlock();
}

void ConnectionManager::RemoveAsset(int id)
{
    removalMutex.lock();
    m_assetsToRemove.emplace_back(id);
    removalMutex.unlock();
}

void ConnectionManager::SendAssets(std::vector<std::shared_ptr<Asset>> assets, bool playerJoined)
{
    if (assets.empty())
    {
        return;
    }

    json j = {{"type", "updateData"}};

    json assetArray = json::array();

    for (const auto &asset : assets)
    {
        if (asset->ShouldSendFull() == true || playerJoined)
        {
            json assetData = {
                {"id", asset->GetId()},   {"alias", asset->GetAlias()}, {"x", asset->GetX()},
                {"y", asset->GetY()},     {"sizeX", asset->GetSizeX()}, {"sizeY", asset->GetSizeY()},
                {"rot", asset->GetRot()}, {"tint", asset->GetTint()},
            };

            assetArray.push_back(assetData);
        }
        else
        {
            json assetData = {
                {"id", asset->GetId()},
                {"x", asset->GetX()},
                {"y", asset->GetY()},
                {"rot", asset->GetRot()},
            };

            assetArray.push_back(assetData);
        }

        asset->Sent();
    }

    j["assets"] = assetArray;

    for (auto &connection : m_impl->connections)
    {
        m_impl->Send(connection.first, j.dump());
    }
}

void ConnectionManager::SendScore() const
{
    json j = {{"type", "scoreData"}};

    json assetArray = json::array();

    const auto &players = m_playerManager->GetPlayers();
    for (const auto &player : players)
    {
        json assetData = {
            {"id", player->GetMainAssetId()},
            {"score", player->GetScore()},
        };

        assetArray.push_back(assetData);
    }

    j["assets"] = assetArray;

    for (auto &connection : m_impl->connections)
    {
        m_impl->Send(connection.first, j.dump());
    }
}

void ConnectionManager::RemoveAssets()
{
    if (m_assetsToRemove.empty())
    {
        return;
    }

    json j = {{"type", "removeData"}};

    json assetArray = json::array();

    removalMutex.lock();
    for (auto const &id : m_assetsToRemove)
    {
        json assetData = {{"id", id}};
        assetArray.push_back(assetData);
    }
    m_assetsToRemove.clear();
    removalMutex.unlock();

    j["assets"] = assetArray;

    for (auto &connection : m_impl->connections)
    {
        m_impl->Send(connection.first, j.dump());
    }
}

std::string ConnectionManager::CreateStatusMessage(std::string message)
{
    json j = {
        {"type", "status"},
        {"status", message},
    };

    return j.dump();
}

std::string ConnectionManager::CreateErrorMessage(std::string message)
{
    json j = {
        {"type", "error"},
        {"error", message},
    };

    return j.dump();
}
