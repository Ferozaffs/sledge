#include "B2Manager.h"
#include "ConnectionManager.h"
#ifdef WIN32
#include "Debugger.h"
#endif
#include "LevelManager.h"
#include "Player.h"
#include "PlayerManager.h"

#include <chrono>
#include <cstring>
#include <memory>

#ifdef WIN32
static std::unique_ptr<Debug::Debugger> debugger = nullptr;
#endif
static std::unique_ptr<Physics::B2Manager> b2Manager = nullptr;
static std::unique_ptr<Gameplay::LevelManager> levelManager = nullptr;
static std::unique_ptr<Gameplay::PlayerManager> playerManager = nullptr;
static std::unique_ptr<Network::ConnectionManager> connectionManager = nullptr;

void Update(const float &deltaTime)
{
#ifdef WIN32
    if (debugger != nullptr)
    {
        debugger->Update(deltaTime);

        // auto player = playerManager->GetPlayer(0);
        // if (player != nullptr &&
        //     (debugger->DbgSledgeInput != 0.0f || debugger->DbgJumpInput != 0.0f || debugger->DbgMoveInput != 0.0f))
        //{
        //     player->SetInputs(debugger->DbgSledgeInput, debugger->DbgMoveInput, debugger->DbgJumpInput);
        // }
    }
#endif

    levelManager->Update(deltaTime);
    playerManager->Update(deltaTime);
    connectionManager->Update(deltaTime);
    b2Manager->Update(deltaTime);
}

void Render()
{
#ifdef WIN32
    if (debugger != nullptr)
    {
        b2Manager->DbgRender(debugger.get());

        debugger->Render();
    }
#endif
}

int main(int argc, char *argv[])
{
#if defined(_WIN32)
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif

    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0)
        {
#ifdef WIN32
            debugger = std::make_unique<Debug::Debugger>();
#endif
        }
    }

    b2Manager = std::make_unique<Physics::B2Manager>();
    levelManager = std::make_unique<Gameplay::LevelManager>(b2Manager->GetWorld());
    playerManager = std::make_unique<Gameplay::PlayerManager>(levelManager.get(), b2Manager->GetWorld());
    connectionManager = std::make_unique<Network::ConnectionManager>(playerManager.get(), levelManager.get());

    levelManager->LoadPlaylist("data/levels");

    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = endTime - startTime;
        startTime = std::chrono::high_resolution_clock::now();

        Update(deltaTime.count());
        Render();
    }

    connectionManager.reset();
    playerManager.reset();
    b2Manager.reset();

#ifdef WIN32
    debugger.reset();
#endif
}
