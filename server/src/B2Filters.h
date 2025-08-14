#pragma once
enum class CollisionFilter : int
{
    Avatar_Head = 0x8000,
    Avatar_Legs = 0x2000,
    Avatar_Body = 0x1000,

    Weapon_Head = 0x0800,
    Weapon_Shaft = 0x0100,

    Block_Decor = 0x0080,
};

enum class BodyType
{
    LevelBlock,
    Avatar,
};

struct PhysicsObjectUserData
{
    BodyType type;
    void *object;
};
