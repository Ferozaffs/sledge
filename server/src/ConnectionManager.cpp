#include "ConnectionManager.h"
#include "Asset.h"
#include "GameManager.h"
#include "WebsocketHelper.h"

#include <nlohmann/json.hpp>

#include <functional>
#include <map>
#include <mutex>
#include <string>

using namespace Network;
using namespace Gameplay;

using json = nlohmann::json;
using namespace nlohmann::literals;

std::vector<int> ConnectionManager::m_assetsToRemove;
std::mutex ConnectionManager::removalMutex;

ConnectionManager::ConnectionManager(Gameplay::GameManager &gameManager)
    : m_gameManager(gameManager), m_tickCounter(0.0f), m_cachedScore(0)
{
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

    WebSocketHelper::connectionMutex.lock();
    bool playerJoined = false;
    while (WebSocketHelper::GetInstance().HasPendingConnections())
    {
        auto player = m_gameManager.AddPlayer();
        WebSocketHelper::GetInstance().InsertPlayerHandle(player);
        playerJoined = true;
    }

    if (tick == true || playerJoined)
    {
        auto assets = m_gameManager.GetAssets(playerJoined);

        SendAssets(assets, playerJoined);

        if (playerJoined == false || m_gameManager.GetScore() != m_cachedScore)
        {
            SendScore();
            m_cachedScore = m_gameManager.GetScore();
        }

        RemoveAssets();
    }

    WebSocketHelper::connectionMutex.unlock();
}

void ConnectionManager::RemoveAsset(int id)
{
    removalMutex.lock();
    m_assetsToRemove.emplace_back(id);
    removalMutex.unlock();
}

void ConnectionManager::SendAssets(std::vector<std::weak_ptr<Asset>> assets, bool playerJoined)
{
    if (assets.empty())
    {
        return;
    }

    json j = {{"type", "updateData"}};

    json assetArray = json::array();

    for (const auto &asset : assets)
    {
        if (auto a = asset.lock())
        {
            if (a->ShouldSendFull() == true || playerJoined)
            {
                json assetData = {
                    {"id", a->GetId()},       {"alias", a->GetAlias()}, {"x", a->GetX()},     {"y", a->GetY()},
                    {"sizeX", a->GetSizeX()}, {"sizeY", a->GetSizeY()}, {"rot", a->GetRot()}, {"tint", a->GetTint()},
                };

                assetArray.push_back(assetData);
            }
            else
            {
                json assetData = {
                    {"id", a->GetId()},
                    {"x", a->GetX()},
                    {"y", a->GetY()},
                    {"rot", a->GetRot()},
                };

                assetArray.push_back(assetData);
            }

            a->Sent();
        }
    }

    j["assets"] = assetArray;

    WebSocketHelper::GetInstance().SendAll(j.dump());
}

void ConnectionManager::SendScore() const
{
    json j = {{"type", "scoreData"}};

    json assetArray = json::array();

    const auto &scoreMap = m_gameManager.GetScoreMap();
    for (const auto &score : scoreMap)
    {
        json assetData = {
            {"id", score.first},
            {"score", score.second},
        };

        assetArray.push_back(assetData);
    }

    j["assets"] = assetArray;

    WebSocketHelper::GetInstance().SendAll(j.dump());
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

    WebSocketHelper::GetInstance().SendAll(j.dump());
}
