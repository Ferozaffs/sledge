#include "Debugger.h"
#include "B2Manager.h"
#include "LevelLoader.h"
#include "PlayerManager.h"
#include "Player.h"
#include "ConnectionManager.h"

#include <memory>
#include <chrono>
#include <GLFW/glfw3.h>


static std::unique_ptr<Debug::Debugger> debugger = nullptr;
static std::unique_ptr<Physics::B2Manager> b2Manager = nullptr;
static std::unique_ptr<Gameplay::LevelLoader> levelLoader = nullptr;
static std::unique_ptr<Gameplay::PlayerManager> playerManager = nullptr;
static std::unique_ptr<Network::ConnectionManager> connectionManager = nullptr;

void Update(const float& deltaTime)
{
    if (debugger != nullptr) {
        debugger->Update(deltaTime);

        auto player = playerManager->GetPlayer(0);
        if (player != nullptr)
        {
            player->SetInputs(debugger->DbgSledgeInput, debugger->DbgJumpInput, debugger->DbgMoveInput);
        }
    }

    playerManager->Update(deltaTime);
    b2Manager->Update(deltaTime);
}

void Render() 
{
    if (debugger != nullptr) {
        b2Manager->DbgRender(debugger.get());

        debugger->Render();
    }
}

int main(int argc, char* argv[])
{
#if defined(_WIN32)
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i],"--debug") == 0 || strcmp(argv[i],"-d") == 0) {
            debugger = std::make_unique<Debug::Debugger>();
        }
    }

    b2Manager = std::make_unique<Physics::B2Manager>();
    levelLoader = std::make_unique<Gameplay::LevelLoader>(b2Manager->GetWorld());
    playerManager = std::make_unique<Gameplay::PlayerManager>(b2Manager->GetWorld());
    connectionManager = std::make_unique<Network::ConnectionManager>(playerManager.get());

    levelLoader->LoadLevel("data/levels/testlevel.bmp");

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
    debugger.reset();
}


