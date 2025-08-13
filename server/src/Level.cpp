#include "Level.h"
#include "LevelBlock.h"

#include <cfloat>
#include <fstream>
#include <nlohmann/json.hpp>
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

Level::Level(std::weak_ptr<b2World> world, const std::pair<std::string, std::string> &files) : m_world(world)
{
    SetDefault();
    LoadSettings(files.second);
    LoadLevel(files.first);
}

Level::~Level()
{
    m_blocks.clear();
    m_spawns.clear();
}

void Level::Update(float deltaTime)
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

void Level::SetDefault()
{

    GameModeConfiguration gameModeConfiguration;
    gameModeConfiguration.invincibility = false;
    gameModeConfiguration.gravityModifier = 1.0f;
    gameModeConfiguration.dampingModifier = 0.0f;
    gameModeConfiguration.frictionModifier = 1.0f;
    gameModeConfiguration.controlModifier = 1.0f;
    gameModeConfiguration.airControl = PlayerControl::Semi;
    gameModeConfiguration.groundControl = PlayerControl::Full;
    gameModeConfiguration.scoringType = ScoringType::LastStanding;
    gameModeConfiguration.respawnsEnabled = false;
    gameModeConfiguration.teams = false;
    m_settings.gameModeConfigurations.push_back(gameModeConfiguration);

    gameModeConfiguration.teams = true;
    m_settings.gameModeConfigurations.push_back(gameModeConfiguration);

    BlockConfiguration staticBlock;
    staticBlock.blockCode = 0x000000;
    staticBlock.assetName = "block_static";
    staticBlock.type = BlockType::Static;
    staticBlock.collision = true;
    staticBlock.destructable = false;
    staticBlock.density = 1.0f;
    staticBlock.friction = 0.1f;
    staticBlock.toughness = FLT_MAX;
    staticBlock.allowPickup = false;

    BlockConfiguration toughBlock;
    toughBlock.blockCode = 0x7F7F7F;
    toughBlock.assetName = "block_tough";
    toughBlock.type = BlockType::Dynamic;
    toughBlock.collision = true;
    toughBlock.destructable = false;
    toughBlock.density = 1.0f;
    toughBlock.friction = 0.1f;
    toughBlock.toughness = 1.0f;
    toughBlock.allowPickup = false;

    BlockConfiguration weakBlock;
    weakBlock.blockCode = 0xB97A57;
    weakBlock.assetName = "block_weak";
    weakBlock.type = BlockType::Dynamic;
    weakBlock.collision = true;
    weakBlock.destructable = true;
    weakBlock.density = 1.0f;
    weakBlock.friction = 0.1f;
    weakBlock.toughness = 0.2f;
    weakBlock.allowPickup = false;

    BlockConfiguration spawnBlock;
    spawnBlock.blockCode = 0xED1C24;
    spawnBlock.type = BlockType::Spawn;
    spawnBlock.spawnType = AffectedPlayers::All;

    m_blockConfigurations = {{staticBlock.blockCode, staticBlock},
                             {toughBlock.blockCode, toughBlock},
                             {weakBlock.blockCode, weakBlock},
                             {spawnBlock.blockCode, spawnBlock}};

    m_settings.autoDecorate = true;
}

bool Level::IsValid()
{
    return m_blocks.empty() == false;
}

const GameSettings &Level::GetSettings() const
{
    return m_settings;
}

const std::map<std::pair<int, int>, std::shared_ptr<LevelBlock>> &Level::GetBlocks() const
{
    return m_blocks;
}

std::vector<std::shared_ptr<LevelBlock>> Gameplay::Level::GetBlocks(unsigned int code)
{
    std::vector<std::shared_ptr<LevelBlock>> blocks;
    for (auto &block : m_blocks)
    {
        if (block.second->GetCode() == code)
        {
            blocks.emplace_back(block.second);
        }
    }

    return blocks;
}

unsigned int Gameplay::Level::GetNumBlocks() const
{
    return m_blocks.size();
}

unsigned int Level::GetNumBlocks(unsigned int code) const
{
    unsigned int count = 0;
    for (const auto &block : m_blocks)
    {
        if (block.second->GetCode() == code)
        {
            count++;
        }
    }

    return count;
}

const std::vector<std::pair<int, int>> &Level::GetSpawns() const
{
    return m_spawns;
}

const std::vector<std::pair<int, int>> &Level::GetRedSpawns() const
{
    return m_redSpawns;
}

const std::vector<std::pair<int, int>> &Level::GetBlueSpawns() const
{
    return m_blueSpawns;
}

void Level::SetGameModeConfiguration(const GameModeConfiguration &configuration)
{
    for (auto &block : m_blocks)
    {
        block.second->SetGameModeConfiguration(configuration);
    }
}

bool Level::LoadLevel(const std::string &levelFilename)
{
    std::ifstream file(levelFilename, std::ios::binary);
    if (file.is_open() == false)
    {
        printf("Error opening file: %s\n", levelFilename.c_str());
        return false;
    }

    BMPHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));

    if (header.signature[0] != 'B' || header.signature[1] != 'M')
    {
        printf("Not a valid .bmp file\n");
        return false;
    }

    if (header.bitsPerPixel != 24)
    {
        printf("Only 24-bit .bmp files are supported\n");
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

bool Level::BuildLevel(std::vector<std::vector<uint8_t>> rows)
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

            auto it = m_blockConfigurations.find(sum);
            if (it != m_blockConfigurations.end())
            {
                if (it->second.type == BlockType::Spawn)
                {
                    if (it->second.spawnType == AffectedPlayers::Red)
                    {
                        m_redSpawns.emplace_back(std::make_pair(col, row));
                    }
                    else if (it->second.spawnType == AffectedPlayers::Blue)
                    {
                        m_blueSpawns.emplace_back(std::make_pair(col, row));
                    }
                    else
                    {
                        m_spawns.emplace_back(std::make_pair(col, row));
                    }
                }
                else
                {
                    CreateBlock(col, row, it->second);
                }
            }
        }
    }

    if (m_settings.autoDecorate)
    {
        Decorate();
    }

    return true;
}

bool Level::CreateBlock(int x, int y, const BlockConfiguration &configuration)
{
    m_blocks.insert(std::make_pair(std::make_pair(x, y), std::make_shared<LevelBlock>(m_world, x, y, configuration)));

    return true;
}

bool Level::Decorate()
{
    static std::random_device rd;
    static std::mt19937 engine(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::set<std::pair<int, int>> coords;
    for (const auto &block : m_blocks)
    {
        if (block.second->HasCollision())
        {
            coords.insert(block.first);
        }
    }

    constexpr float decorChance = 0.75f;
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

bool Level::CreateFloorDecor(std::pair<int, int> coord)
{
    constexpr unsigned int numDecor = 3;
    BlockConfiguration decorBlock;
    decorBlock.blockCode = 0xABCD1234;
    decorBlock.assetName = "block_floor_decor_" + std::to_string((rand() % numDecor) + 1);
    decorBlock.type = BlockType::Decor;
    decorBlock.collision = true;
    decorBlock.destructable = true;
    decorBlock.density = 1.0f;
    decorBlock.friction = 0.1f;
    decorBlock.toughness = 0.0f;
    decorBlock.allowPickup = false;

    CreateBlock(coord.first, coord.second, decorBlock);
    return true;
}

bool Level::CreateRoofDecor(std::pair<int, int> coord)
{
    constexpr unsigned int numDecor = 2;
    BlockConfiguration decorBlock;
    decorBlock.blockCode = 0xABCD1234;
    decorBlock.assetName = "block_roof_decor_" + std::to_string((rand() % numDecor) + 1);
    decorBlock.type = BlockType::Decor;
    decorBlock.collision = true;
    decorBlock.destructable = true;
    decorBlock.density = 1.0f;
    decorBlock.friction = 0.1f;
    decorBlock.toughness = 0.0f;
    decorBlock.allowPickup = false;

    CreateBlock(coord.first, coord.second, decorBlock);
    return true;
}

bool Level::LoadSettings(const std::string &settingsFilename)
{
    std::ifstream file(settingsFilename);

    if (file.is_open() == false)
    {
        printf("No settings file, using defaults: %s\n", settingsFilename.c_str());
        return false;
    }

    try
    {
        nlohmann::json j = nlohmann::json::parse(file);

        if (j.contains("gameModeConfigurations") && j["gameModeConfigurations"].is_array())
        {
            m_settings.gameModeConfigurations.clear();
            for (const auto &mode : j["gameModeConfigurations"])
            {
                Gameplay::GameModeConfiguration config{};

                if (mode.contains("invincibility"))
                {
                    config.invincibility = mode["invincibility"];
                }
                if (mode.contains("gravityModifier"))
                {
                    config.gravityModifier = mode["gravityModifier"];
                }
                if (mode.contains("dampingModifier"))
                {
                    config.dampingModifier = mode["dampingModifier"];
                }
                if (mode.contains("frictionModifier"))
                {
                    config.frictionModifier = mode["frictionModifier"];
                }
                if (mode.contains("airControl"))
                {
                    config.airControl = static_cast<PlayerControl>(mode["airControl"].get<unsigned int>());
                }
                if (mode.contains("groundControl"))
                {
                    config.groundControl = static_cast<PlayerControl>(mode["groundControl"].get<unsigned int>());
                }
                if (mode.contains("controlModifier"))
                {
                    config.controlModifier = mode["controlModifier"];
                }

                config.teams = mode["teams"];
                config.respawnsEnabled = mode["respawnsEnabled"];
                config.respawnTime = mode["respawnTime"];
                config.scoringType = static_cast<Gameplay::ScoringType>(mode["scoringType"].get<unsigned int>());
                config.pointsToWin = mode["pointsToWin"];
                config.scalePointsToPlayers = mode["scalePointsToPlayers"];

                if (mode.contains("scoreObjective") && mode["scoreObjective"].is_array())
                {
                    for (const auto &scoreObjective : mode["scoreObjective"])
                    {
                        Gameplay::GameScoreObjective objective{};

                        objective.objectCode = scoreObjective["objectCode"];
                        objective.triggerCode = scoreObjective["triggerCode"];
                        objective.objectiveType =
                            static_cast<Gameplay::ObjectiveType>(scoreObjective["objectiveType"].get<unsigned int>());
                        objective.triggerType =
                            static_cast<Gameplay::TriggerType>(scoreObjective["triggerType"].get<unsigned int>());
                        objective.affectedPlayers = static_cast<Gameplay::AffectedPlayers>(
                            scoreObjective["affectedPlayers"].get<unsigned int>());
                        objective.scoreTickRate = scoreObjective["scoreTickRate"];
                        objective.resetObject = scoreObjective["resetObject"];

                        config.scoreObjectives.push_back(objective);
                    }
                }
                m_settings.gameModeConfigurations.push_back(config);
            }
        }

        if (j.contains("blockConfigurations") && j["blockConfigurations"].is_array())
        {
            m_settings.blockConfigurations.clear();
            for (const auto &blockConfiguration : j["blockConfigurations"])
            {
                Gameplay::BlockConfiguration block{};

                block.blockCode = blockConfiguration["blockCode"];
                block.assetName = blockConfiguration["assetName"];
                block.type = static_cast<Gameplay::BlockType>(blockConfiguration["type"].get<unsigned int>());
                block.spawnType =
                    static_cast<Gameplay::AffectedPlayers>(blockConfiguration["spawnType"].get<unsigned int>());
                block.collision = blockConfiguration["collision"];
                block.destructable = blockConfiguration["destructable"];
                block.density = blockConfiguration["density"];
                block.friction = blockConfiguration["friction"];
                block.toughness = blockConfiguration["toughness"];
                if (blockConfiguration.contains("restitution"))
                {
                    block.restitution = blockConfiguration["restitution"];
                }
                block.allowPickup = blockConfiguration["allowPickup"];

                m_settings.blockConfigurations.push_back(block);
            }
        }

        if (j.contains("autoDecorate"))
        {
            m_settings.autoDecorate = j["autoDecorate"];
        }

        for (const auto &blockConfiguration : m_settings.blockConfigurations)
        {
            m_blockConfigurations[blockConfiguration.blockCode] = blockConfiguration;
        }
    }
    catch (const std::exception &e)
    {
        printf("Json file failed to load (%s), using defaults: %s\n", e.what(), settingsFilename.c_str());
        return false;
    }

    return true;
}
