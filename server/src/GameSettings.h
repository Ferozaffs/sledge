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

enum class Team : unsigned int
{
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
    bool invincibility;
    float gravityModifier;
    float dampingModifier;
    float frictionModifier;
    PlayerControl airControl;
    PlayerControl groundControl;

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
    Zone,
    Decor,
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
    std::vector<GameModeConfiguration> gameModeConfigurations;
    std::vector<BlockConfiguration> blockConfigurations;
    bool autoDecorate;
};

} // namespace Gameplay
