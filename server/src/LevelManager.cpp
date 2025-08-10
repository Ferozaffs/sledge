#include "LevelManager.h"
#include "Asset.h"
#include "GameManager.h"
#include "Level.h"
#include "LevelBlock.h"

#include <box2d/box2d.h>
#include <filesystem>

namespace fs = std::filesystem;
using namespace Gameplay;

LevelManager::LevelManager(std::weak_ptr<b2World> world) : m_world(world), m_currentLevelIndex(0), m_reloaded(false)
{
}

LevelManager::~LevelManager()
{
}

bool LevelManager::LoadPlaylist(const std::string &path)
{
    fs::path p(path);

    if (fs::exists(p))
    {
        printf("Loaded playlist from: %s\n", path.c_str());
        if (fs::is_directory(p))
        {
            for (const auto &level : fs::directory_iterator(p))
            {
                if (level.is_regular_file() && level.path().extension() == ".bmp")
                {
                    auto levelName = level.path();
                    std::string settingsName = "";
                    for (const auto &settings : fs::directory_iterator(p))
                    {
                        if (settings.is_regular_file() && settings.path().stem() == levelName.stem() &&
                            settings.path().extension() == ".json")
                        {
                            settingsName = settings.path().generic_string();
                        }
                    }

                    m_playlist.emplace_back(levelName.generic_string(), settingsName);
                }
            }

            if (m_playlist.empty() == false)
            {
                return LoadLevel(m_playlist.back());
            }

            printf("No files of type .bmp in directory\n");
            return false;
        }
        else if (fs::is_regular_file(p))
        {
            if (p.extension() == ".bmp")
            {
                m_playlist.emplace_back(p.generic_string(), "");
                return LoadLevel(m_playlist.back());
            }
            printf("File is not of type .bmp\n");
            return false;
        }

        printf("Path is neither file or directory\n");
        return false;
    }
    else
    {
        printf("File or directory %s does not exist\n", p.generic_string().c_str());
        return false;
    }
}

void LevelManager::Update(float deltaTime)
{
    m_currentLevel->Update(deltaTime);
}

std::vector<std::weak_ptr<Asset>> LevelManager::GetAssets(bool allAssets)
{
    std::vector<std::weak_ptr<Asset>> assets;
    if (m_currentLevel != nullptr)
    {
        for (const auto &block : m_currentLevel->GetBlocks())
        {
            if (auto b = block.second->GetAsset().lock())
            {
                if (allAssets || m_reloaded || block.second->InMotion() || b->ShouldSendFull())
                {
                    assets.emplace_back(b);
                }
            }
        }

        m_reloaded = false;
    }

    return assets;
}

std::vector<std::pair<int, int>> Gameplay::LevelManager::GetSpawns() const
{
    return m_currentLevel != nullptr ? m_currentLevel->GetSpawns() : std::vector<std::pair<int, int>>();
}

GameSettings LevelManager::GetSettings() const
{
    return m_currentLevel != nullptr ? m_currentLevel->GetSettings() : GameSettings();
}

bool LevelManager::LoadLevel(const std::pair<std::string, std::string> &files)
{
    m_currentLevel = std::make_unique<Level>(m_world, m_playlist.back());
    return m_currentLevel->IsValid();
}

bool LevelManager::NextLevel()
{
    if (m_playlist.size() == 0)
    {
        return false;
    }

    ++m_currentLevelIndex;
    auto numLevels = m_playlist.size();

    while (numLevels > 0)
    {
        if (LoadLevel(m_playlist[m_currentLevelIndex % m_playlist.size()]))
        {
            break;
        }
        ++m_currentLevelIndex;
        --numLevels;
    }

    m_reloaded = true;

    return true;
}
