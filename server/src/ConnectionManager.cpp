#include "ConnectionManager.h"
#include "Asset.h"
#include "GameManager.h"
#include "Packet.h"
#include "WebsocketHelper.h"

using namespace Network;
using namespace Gameplay;

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
    WebSocketHelper::connectionMutex.unlock();

    if (tick == true || playerJoined)
    {
        auto assets = m_gameManager.GetAssets(playerJoined);

        SendAssets(assets, playerJoined);

        if (playerJoined == false || m_gameManager.GetScore() != m_cachedScore)
        {
            SendScore();
            m_cachedScore = m_gameManager.GetScore();
        }
        if (playerJoined == false || m_gameManager.GetPoints() != m_cachedPoints)
        {
            SendPoints();
            m_cachedPoints = m_gameManager.GetPoints();
        }

        RemoveAssets();
    }
}

void ConnectionManager::RemoveAsset(int id)
{
    removalMutex.lock();
    m_assetsToRemove.emplace_back(id);
    removalMutex.unlock();
}

void ConnectionManager::SendAssets(std::vector<std::weak_ptr<Asset>> assets, bool playerJoined)
{
    if (assets.empty() == true)
    {
        return;
    }

    std::vector<Packet::GameAsset> assetCreation;
    std::vector<Packet::GameAsset> assetUpdate;

    for (const auto &asset : assets)
    {
        if (auto a = asset.lock())
        {
            if (a->ShouldSendFull() == true || playerJoined)
            {
                auto ga = Packet::GameAsset();
                ga.id = a->GetId();
                ga.x = a->GetX();
                ga.y = a->GetY();
                ga.rot = a->GetRot();
                ga.sizeX = a->GetSizeX();
                ga.sizeY = a->GetSizeY();
                ga.tint = a->GetTint();
                ga.aliasLength = a->GetAlias().size();
                ga.alias = a->GetAlias();

                assetCreation.emplace_back(ga);
            }
            else if (a->ShouldUpdate())
            {
                auto ga = Packet::GameAsset();
                ga.id = a->GetId();
                ga.x = a->GetX();
                ga.y = a->GetY();
                ga.rot = a->GetRot();

                assetUpdate.emplace_back(ga);
            }

            a->Sent();
        }
    }

    WebSocketHelper::connectionMutex.lock();
    if (assetCreation.empty() == false)
    {
        WebSocketHelper::GetInstance().SendAll(Packet::CreateAssetPacket(Packet::AssetCommand::Create, assetCreation));
    }
    if (assetUpdate.empty() == false)
    {
        WebSocketHelper::GetInstance().SendAll(Packet::CreateAssetPacket(Packet::AssetCommand::Update, assetUpdate));
    }
    WebSocketHelper::connectionMutex.unlock();
}

void ConnectionManager::SendScore() const
{
    std::vector<Packet::GameScore> scores;

    const auto &scoreMap = m_gameManager.GetScoreMap();
    for (const auto &score : scoreMap)
    {
        auto gs = Packet::GameScore();
        gs.id = score.first;
        gs.score = score.second;

        scores.emplace_back(gs);
    }

    WebSocketHelper::connectionMutex.lock();
    if (scores.empty() == false)
    {
        WebSocketHelper::GetInstance().SendAll(Packet::CreateScorePacket(scores));
    }
    WebSocketHelper::connectionMutex.unlock();
}

void ConnectionManager::SendPoints() const
{
    std::vector<Packet::GamePoints> points;

    const auto &pointsMap = m_gameManager.GetPointsMap();
    for (const auto &point : pointsMap)
    {
        auto gs = Packet::GamePoints();
        gs.id = point.first;
        gs.fraction = point.second;

        points.emplace_back(gs);
    }

    WebSocketHelper::connectionMutex.lock();
    if (points.empty() == false)
    {
        WebSocketHelper::GetInstance().SendAll(Packet::CreatePointsPacket(points));
    }
    WebSocketHelper::connectionMutex.unlock();
}

void ConnectionManager::RemoveAssets()
{
    if (m_assetsToRemove.empty())
    {
        return;
    }

    std::vector<Packet::GameAsset> assets;

    removalMutex.lock();
    for (auto const &id : m_assetsToRemove)
    {
        auto ga = Packet::GameAsset();
        ga.id = id;
        assets.emplace_back(ga);
    }
    m_assetsToRemove.clear();
    removalMutex.unlock();

    WebSocketHelper::connectionMutex.lock();
    if (assets.empty() == false)
    {
        WebSocketHelper::GetInstance().SendAll(Packet::CreateAssetPacket(Packet::AssetCommand::Remove, assets));
    }
    WebSocketHelper::connectionMutex.unlock();
}
