#include "LevelManager.h"
#include "LevelBlock.h"

#include <box2d/box2d.h>
#include <fstream>
#include <random>
#include <set>

using namespace Gameplay;

#pragma pack(push, 1)
struct BMPHeader
{
    char signature[2];
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixelDataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compressionMethod;
    uint32_t imageSize;
    int32_t horizontalPixelsPerMeter;
    int32_t verticalPixelsPerMeter;
    uint32_t colorsInPalette;
    uint32_t importantColors;
};
#pragma pack(pop)

std::map<unsigned int, std::string> assetMap = {
    {0x000000, "block_static"}, {0x7F7F7F, "block_tough"}, {0xB97A57, "block_weak"}, {0xED1C24, "spawn"}};

LevelManager::LevelManager(const std::shared_ptr<b2World> &world)
    : m_world(world), m_currentLevel(""), m_reloaded(false)
{
}

LevelManager::~LevelManager()
{
}

bool LevelManager::LoadLevel(const std::string &filename)
{
    m_currentLevel = filename;

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        printf_s("Error opening file: %s\n", filename.c_str());
        return false;
    }

    BMPHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));

    if (header.signature[0] != 'B' || header.signature[1] != 'M')
    {
        printf_s("Not a valid BMP file\n");
        return false;
    }

    if (header.bitsPerPixel != 24)
    {
        printf_s("Only 24-bit BMP files are supported\n");
        return false;
    }
    file.seekg(header.pixelDataOffset);

    const int padding = (4 - (header.width * 3) % 4) % 4;

    std::vector<std::vector<uint8_t>> rows;
    for (int i = 0; i < header.height; i++)
    {
        std::vector<uint8_t> pixelData(header.width * 3);

        file.read(reinterpret_cast<char *>(pixelData.data()), pixelData.size());
        file.seekg(padding, std::ios_base::cur);

        rows.emplace_back(pixelData);
    }

    return BuildLevel(rows);
}

bool LevelManager::ReloadLevel()
{
    m_blocks.clear();
    m_spawns.clear();
    LoadLevel(m_currentLevel);

    m_reloaded = true;

    return true;
}

void LevelManager::Update(float deltaTime)
{
    for (auto it = m_blocks.begin(); it != m_blocks.end();)
    {
        if (it->second->Update(deltaTime) == false)
        {
            it = m_blocks.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::vector<std::shared_ptr<Asset>> LevelManager::GetAssets() const
{
    std::vector<std::shared_ptr<Asset>> assets;
    for (const auto &block : m_blocks)
    {
        const auto &blockAsset = block.second->GetAsset();
        if (blockAsset != nullptr)
        {
            assets.emplace_back(blockAsset);
        }
    }

    return assets;
}

std::vector<std::shared_ptr<Asset>> LevelManager::GetDynamicAssets()
{
    std::vector<std::shared_ptr<Asset>> assets;
    for (const auto &block : m_blocks)
    {
        const auto &blockAsset = block.second->GetAsset();
        if (blockAsset != nullptr && (block.second->InMotion() || m_reloaded))
        {
            assets.emplace_back(blockAsset);
        }
    }

    m_reloaded = false;

    return assets;
}

const std::vector<std::pair<int, int>> &Gameplay::LevelManager::GetSpawns() const
{
    return m_spawns;
}

bool LevelManager::BuildLevel(std::vector<std::vector<uint8_t>> rows)
{
    for (int row = 0; row < rows.size(); row++)
    {
        auto cols = rows[row].size() / 3;
        for (int col = 0; col < cols; col++)
        {
            auto index = col * 3;
            uint8_t blue = rows[row][index];
            uint8_t green = rows[row][index + 1];
            uint8_t red = rows[row][index + 2];

            unsigned int sum = (red << 16) + (green << 8) + blue;

            auto it = assetMap.find(sum);
            if (it != assetMap.end())
            {
                if (it->second == "spawn")
                {
                    m_spawns.emplace_back(std::make_pair(col, row));
                }
                else
                {
                    CreateBlock(col, row, it->second);
                }
            }
        }
    }

    Decorate();

    return true;
}

bool LevelManager::CreateBlock(int x, int y, std::string alias)
{
    m_blocks.insert(std::make_pair(std::make_pair(x, y), std::make_shared<LevelBlock>(m_world.get(), x, y, alias)));

    return true;
}

bool LevelManager::Decorate()
{
    static std::random_device rd;
    static std::mt19937 engine(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::set<std::pair<int, int>> coords;
    for (const auto &block : m_blocks)
    {
        coords.insert(block.first);
    }

    constexpr float decorChance = 0.5f;
    for (const auto &coord : coords)
    {
        if (dist(engine) < decorChance)
        {
            auto floorCoord = std::make_pair(coord.first, coord.second + 1);

            if (coords.find(floorCoord) == coords.end())
            {
                CreateFloorDecor(floorCoord);
            }
        }

        if (dist(engine) < decorChance)
        {
            auto roofCoord = std::make_pair(coord.first, coord.second - 1);
            if (coords.find(roofCoord) == coords.end())
            {
                CreateRoofDecor(roofCoord);
            }
        }
    }

    return true;
}

bool LevelManager::CreateFloorDecor(std::pair<int, int> coord)
{
    CreateBlock(coord.first, coord.second, "block_floor_decor");
    return true;
}

bool LevelManager::CreateRoofDecor(std::pair<int, int> coord)
{
    CreateBlock(coord.first, coord.second, "block_roof_decor");
    return true;
}
