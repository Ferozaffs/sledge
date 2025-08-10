#pragma once
#include <string>
#include <vector>

namespace Gameplay
{
enum class ObjectiveType : unsigned int
{
    Zone,
    Destruction
};
enum class TriggerType : unsigned int
{
    Player,
    Object
};

enum class ScoringType : unsigned int
{
    LastStanding,
    Deaths,
    Objectives
};

enum class PlayerControl : unsigned int
{
    Off,
    Semi,
    Full
};

enum class AffectedPlayers : unsigned int
{
    All,
    Blue,
    Red,
};

struct GameScoreObjective
{
    unsigned int objectCode;
    unsigned int triggerCode;
    ObjectiveType objectiveType;
    TriggerType triggerType;
    AffectedPlayers affectedPlayers;
    float scoreTickRate;
    bool resetObject;
};

struct GameModeConfiguration
{
    bool teams;
    bool respawnsEnabled;
    float respawnTime;
    ScoringType scoringType;
    int pointsToWin;

    std::vector<GameScoreObjective> scoreObjectives;
};

enum class BlockType : unsigned int
{
    Static,
    Dynamic,
    Spawn,
};

struct BlockConfiguration
{
    unsigned int blockCode;
    std::string assetName;
    BlockType type;
    AffectedPlayers spawnType;
    bool collision;
    bool destructable;
    float density;
    float friction;
    float toughness;
    bool allowPickup;
};

struct GameSettings
{
    bool invincibility;
    float gravityModifier;
    float dampingModifier;
    float frictionModifier;
    PlayerControl airControl;
    PlayerControl groundControl;

    std::vector<GameModeConfiguration> gameModeConfigurations;
    std::vector<BlockConfiguration> blockConfigurations;
    bool autoDecorate;
};

} // namespace Gameplay
