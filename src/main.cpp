#include "Renderer.h"
#include "B2Manager.h"
#include "Player.h"

#include <memory>
#include <chrono>


static std::unique_ptr<Debug::Renderer> dbgRenderer = nullptr;
static std::unique_ptr<Physics::B2Manager> b2Manager = nullptr;

void Update(const float& deltaTime)
{
    b2Manager->Update(deltaTime);
}

void Render() 
{
    if (dbgRenderer != nullptr) {
        b2Manager->DbgRender(dbgRenderer.get());

        dbgRenderer->Render();
    }
}

int main(int argc, char* argv[])
{
#if defined(_WIN32)
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i],"--debug") == 0 || strcmp(argv[i],"-d") == 0) {
            dbgRenderer = std::make_unique<Debug::Renderer>();
        }
    }

    b2Manager = std::make_unique<Physics::B2Manager>();
    new Gameplay::Player(b2Manager->GetWorld(), b2Vec2(0.0f, 4.0f));

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
}


