#include "Debugger.h"
#include "B2Manager.h"
#include "LevelLoader.h"
#include "Avatar.h"

#include <memory>
#include <chrono>
#include <GLFW/glfw3.h>


static std::unique_ptr<Debug::Debugger> debugger = nullptr;
static std::unique_ptr<Physics::B2Manager> b2Manager = nullptr;
static std::unique_ptr<Gameplay::LevelLoader> levelLoader = nullptr;
static std::unique_ptr<Gameplay::Avatar> avatar = nullptr;

void Update(const float& deltaTime)
{
    float dbgSledgeInput = 0.0f;
    float dbgJumpInput = 0.0f;
    float dbgMoveInput = 0.0f;
    if (debugger != nullptr) {
        debugger->Update(deltaTime);
        dbgSledgeInput = debugger->DbgSledgeInput;
        dbgJumpInput = debugger->DbgJumpInput;
        dbgMoveInput = debugger->DbgMoveInput;
    }

    avatar->Update(deltaTime, dbgSledgeInput, dbgJumpInput, dbgMoveInput);
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
    avatar = std::make_unique<Gameplay::Avatar>(b2Manager->GetWorld(), b2Vec2(0.0f, 4.0f));

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

    debugger.reset();
    b2Manager.reset();
    avatar.reset();
}


