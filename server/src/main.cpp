#include "B2Manager.h"
#include "ConnectionManager.h"
#ifdef WIN32
#include "Debugger.h"
#endif
#include "GameManager.h"
#include "LevelManager.h"
#include "Player.h"
#include "PlayerManager.h"

#include <chrono>
#include <cstring>
#include <memory>

#ifdef WIN32
static std::shared_ptr<Debug::Debugger> debugger = nullptr;
#endif

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
            debugger = std::make_shared<Debug::Debugger>();
#endif
        }
    }

    Physics::B2Manager b2Manager;
    Gameplay::GameManager gameManager(b2Manager.GetWorld());
    Network::ConnectionManager connectionManager(gameManager);

    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = endTime - startTime;
        startTime = std::chrono::high_resolution_clock::now();

        {
            gameManager.Update(deltaTime.count());

            connectionManager.Update(deltaTime.count());

            b2Manager.Update(deltaTime.count());
        }

        {
#ifdef WIN32
            if (debugger != nullptr)
            {
                debugger->Update(deltaTime.count());

                b2Manager.DbgRender(debugger);
                debugger->Render();
            }
#endif
        }
    }

#ifdef WIN32
    debugger.reset();
#endif
}
