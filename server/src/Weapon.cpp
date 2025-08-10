#include "Weapon.h"
#include "Asset.h"
#include "Avatar.h"
#include "B2Filters.h"

#include <box2d/box2d.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

using namespace Gameplay;

std::vector<std::pair<std::string, Weapon::WeaponData>> Weapon::WeaponCache;

bool Weapon::SortRarity(std::pair<std::string, Weapon::WeaponData> &a, std::pair<std::string, Weapon::WeaponData> &b)
{
    return a.second.rarity > b.second.rarity;
}

Weapon::Weapon(const WeaponData &weaponData, std::weak_ptr<b2World> world, const Avatar &avatar)
    : m_speed(0.0f), m_torque(0.0f)
{
    CreateWeapon(weaponData, world, avatar);
}

Weapon::~Weapon()
{
    BreakJoints();
}

void Weapon::Update(float deltaTime)
{
}

float Weapon::GetSpeed() const
{
    return m_speed;
}

float Weapon::GetTorque() const
{
    return m_torque;
}

std::vector<std::shared_ptr<Asset>> Weapon::GetAssets() const
{
    return m_assets;
}

void Weapon::BreakJoints()
{
    while (m_joints.empty() == false)
    {
        m_joints.back()->GetBodyA()->GetWorld()->DestroyJoint(m_joints.back());
        m_joints.pop_back();
    };
}

std::unique_ptr<Weapon> Weapon::GetWeaponFromPool(std::weak_ptr<b2World> world, const Avatar &avatar)
{
    if (WeaponCache.empty())
    {
        LoadWeapons();
    }

    for (const auto &wpn : WeaponCache)
    {
        int random = rand();
        if (random % wpn.second.rarity == 0)
        {
            return std::make_unique<Weapon>(wpn.second, world, avatar);
        }
    }

    return nullptr;
}

void Weapon::LoadWeapons()
{
    for (const auto &entry : std::filesystem::directory_iterator("data/weapons/"))
    {
        if (entry.is_regular_file())
        {
            std::string fileName = entry.path().filename().string();
            if (fileName.rfind("wpn_", 0) == 0)
            {
                LoadWeaponFromJson(entry.path().string());
            }
        }
    }

    sort(WeaponCache.begin(), WeaponCache.end(), SortRarity);
}

Weapon::WeaponData Weapon::LoadWeaponFromJson(const std::string &jsonFile)
{
    for (const auto &wpn : WeaponCache)
    {
        if (wpn.first == jsonFile)
        {
            return wpn.second;
        }
    }

    nlohmann::json j;

    std::ifstream inputFile(jsonFile);
    if (inputFile.is_open() == false)
    {
        printf("Could not open weapon file: %s\n", jsonFile.c_str());
        return WeaponData();
    }

    try
    {
        inputFile >> j;
    }
    catch (nlohmann::json::parse_error &e)
    {
        printf("Parse error: %s\n", e.what());
        return WeaponData();
    }

    inputFile.close();

    WeaponData wpnData;
    wpnData.torque = j["torque"];
    wpnData.speed = j["speed"];
    wpnData.rarity = j["rarity"];

    auto shapes = j["shapes"];
    for (const auto &shape : shapes)
    {
        WeaponShape wpnShape;
        wpnShape.links = shape["links"];
        wpnShape.sizeX = shape["size"]["x"];
        wpnShape.sizeY = shape["size"]["y"];
        wpnShape.offsetX = shape["offset"]["x"];
        wpnShape.offsetY = shape["offset"]["y"];
        wpnShape.anchorX = shape["anchor"]["x"];
        wpnShape.anchorY = shape["anchor"]["y"];
        wpnShape.anchorLimit = shape["anchor"]["limit"];
        wpnShape.anchorType = shape["anchor"]["type"];
        wpnShape.density = shape["density"];
        wpnShape.friction = shape["friction"];
        wpnShape.canDamage = shape["candamage"];
        wpnShape.texture = shape["texture"];

        std::string tintStr = shape["tint"];
        std::stringstream ss;
        ss << std::hex << tintStr;
        ss >> wpnShape.tint;

        wpnData.shapes.push_back(wpnShape);
    }

    WeaponCache.push_back({jsonFile, wpnData});
    return wpnData;
}

void Weapon::CreateWeapon(const WeaponData &wpnData, std::weak_ptr<b2World> world, const Avatar &avatar)
{
    m_torque = wpnData.torque;
    m_speed = wpnData.speed;

    for (const auto &shape : wpnData.shapes)
    {
        CreateShape(shape, world, avatar);
    }
}

void Weapon::CreateShape(const WeaponShape &shape, std::weak_ptr<b2World> world, const Avatar &avatar)
{
    if (auto w = world.lock())
    {
        b2Vec2 orgPos;

        if (m_assets.size() > 0)
        {
            orgPos = m_assets.back()->GetBody()->GetPosition();
            orgPos.x += m_assets.back()->GetSizeX() * 0.5f;
        }
        else
        {
            orgPos = avatar.GetPosition();
            orgPos.x += avatar.GetShaftLength() * 2.0f;
        }

        auto bodyPos = orgPos;
        auto anchorPos = orgPos;

        for (int i = 0; i < shape.links; ++i)
        {
            bodyPos.x += shape.offsetX;
            bodyPos.y += shape.offsetY;

            auto asset = CreateBody(shape, bodyPos, w);

            auto connection = avatar.GetShaft();
            if (m_assets.size() > 0)
            {
                connection = m_assets.back()->GetBody();
            }

            anchorPos.x += shape.anchorX;
            anchorPos.y += shape.anchorY;

            CreateAnchor(shape, anchorPos, w, connection, asset);

            m_assets.emplace_back(asset);

            bodyPos.x += shape.offsetX;
            bodyPos.y += shape.offsetY;
            anchorPos.x += shape.offsetX * 2.0f;
            anchorPos.y += shape.offsetY * 2.0f;
        }
    }
}

std::shared_ptr<Asset> Weapon::CreateBody(const WeaponShape &shape, const b2Vec2 &pos, std::shared_ptr<b2World> world)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pos;
    bodyDef.fixedRotation = false;
    auto asset = std::make_shared<Asset>(world->CreateBody(&bodyDef), shape.texture);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(shape.sizeX, shape.sizeY);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = shape.density;
    fixtureDef.friction = shape.friction;

    if (shape.canDamage == true)
    {
        fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Weapon_Head);
        fixtureDef.filter.maskBits = 0xFFFF;
    }
    else
    {
        fixtureDef.filter.categoryBits = static_cast<unsigned int>(CollisionFilter::Weapon_Shaft);
        fixtureDef.filter.maskBits = 0xFFFF;
        fixtureDef.filter.maskBits &= ~(static_cast<unsigned int>(CollisionFilter::Avatar_Head) |
                                        static_cast<unsigned int>(CollisionFilter::Avatar_Legs));
    }

    asset->GetBody()->CreateFixture(&fixtureDef);
    asset->UpdateSize();

    return asset;
}

void Weapon::CreateAnchor(const WeaponShape &shape, const b2Vec2 &pos, std::shared_ptr<b2World> world,
                          b2Body *connection, const std::shared_ptr<Asset> &asset)
{
    if (shape.anchorType == "weld")
    {
        b2WeldJointDef joint;
        joint.collideConnected = false;
        joint.Initialize(connection, asset->GetBody(), pos);
        m_joints.emplace_back(world->CreateJoint(&joint));
    }
    if (shape.anchorType == "revolute")
    {
        b2RevoluteJointDef joint;
        joint.collideConnected = false;
        joint.enableLimit = true;
        joint.upperAngle = shape.anchorLimit;
        joint.lowerAngle = -shape.anchorLimit;
        joint.Initialize(connection, asset->GetBody(), pos);
        m_joints.emplace_back(world->CreateJoint(&joint));
    }
}
