#include "ConnectionManager.h"
#include "Asset.h"
#include "LevelManager.h"
#include "Player.h"
#include "PlayerManager.h"

#pragma warning(push)
#pragma warning(disable : 4267)
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning(pop)

#include <nlohmann/json.hpp>

#include <Windows.h>
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
std::vector<GUID> ConnectionManager::m_assetsToRemove;
std::mutex connectionMutex;
std::mutex removalMutex;

#pragma warning(push)
#pragma warning(disable : 4244)
std::string guidToString(const GUID &guid)
{
    wchar_t guidWStr[39]; // GUIDs are 38 characters plus null terminator
    StringFromGUID2(guid, guidWStr, 39);
    std::wstring wideStr(guidWStr);
    return std::string(wideStr.begin(), wideStr.end());
}
#pragma warning(pop)

class Network::Impl
{
  public:
    server endpoint;
    std::map<std::shared_ptr<connection>, std::shared_ptr<Gameplay::Player>> connections;

    Impl()
    {
        thread = std::thread(&websocketThread, this);
    }

    void Send(const std::shared_ptr<Gameplay::Player> &player, std::string message)
    {
        for (const auto &connection : connections)
        {
            if (player == connection.second)
            {
                Send(connection.first, message.c_str());
            }
        }
    }

    void Send(std::shared_ptr<connection> connection, std::string message)
    {
        if (connection->get_state() == websocketpp::session::state::open)
        {
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
        endpoint.set_access_channels(websocketpp::log::alevel::none);
        endpoint.clear_access_channels(websocketpp::log::alevel::none);
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
    : m_playerManager(playerManager), m_levelManager(levelManager), m_tickCounter(0.0f)
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
    for (auto &connection : m_impl->connections)
    {
        if (connection.second == nullptr)
        {
            auto player = m_playerManager->CreatePlayer();
            connection.second = player;

            SendAssets(player, m_levelManager->GetAssets());
        }

        if (tick == true)
        {
            auto assets = m_levelManager->GetDynamicAssets();
            auto playerAssets = m_playerManager->GetDynamicAssets();

            assets.insert(assets.end(), playerAssets.begin(), playerAssets.end());
            SendAssets(connection.second, assets);

            RemoveAssets();
        }
    }
    connectionMutex.unlock();
}

void ConnectionManager::RemoveAsset(GUID id)
{
    removalMutex.lock();
    m_assetsToRemove.emplace_back(id);
    removalMutex.unlock();
}

void ConnectionManager::SendAssets(const std::shared_ptr<Player> &player, std::vector<std::shared_ptr<Asset>> assets)
{
    json j = {{"type", "updateData"}};

    json assetArray = json::array();

    for (const auto &asset : assets)
    {
        json assetData = {
            {"id", guidToString(asset->GetId())},
            {"alias", asset->GetAlias()},
            {"x", asset->GetX()},
            {"y", asset->GetY()},
            {"sizeX", asset->GetSizeX()},
            {"sizeY", asset->GetSizeY()},
            {"rot", asset->GetRot()},
            {"tint", asset->GetTint()},
        };

        assetArray.push_back(assetData);
    }

    j["assets"] = assetArray;

    m_impl->Send(player, j.dump());
}

void ConnectionManager::RemoveAssets()
{
    json j = {{"type", "removeData"}};

    json assetArray = json::array();

    removalMutex.lock();
    for (auto const &id : m_assetsToRemove)
    {
        json assetData = {{"id", guidToString(id)}};
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
