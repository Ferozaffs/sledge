#include "Weapon.h"
#include "Asset.h"
#include "Avatar.h"
#include "B2Filters.h"

#include <box2d/box2d.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

using namespace Gameplay;

std::unordered_map<std::string, Weapon::WeaponData> Weapon::WeaponCache;

Weapon::Weapon(const std::string &jsonFile, std::weak_ptr<b2World> world, const Avatar &avatar)
    : m_speed(0.0f), m_torque(0.0f)
{
    auto weaponData = LoadWeaponFromJson(jsonFile);
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

Weapon::WeaponData Weapon::LoadWeaponFromJson(const std::string &jsonFile)
{
    auto it = WeaponCache.find(jsonFile);
    if (it == WeaponCache.end())
    {
        nlohmann::json j;

        std::ifstream inputFile(jsonFile);
        if (!inputFile.is_open())
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

        auto shapes = j["shapes"];
        for (const auto &shape : shapes)
        {
            WeaponShape wpnShape;
            wpnShape.sizeX = shape["size"]["x"];
            wpnShape.sizeY = shape["size"]["y"];
            wpnShape.offsetX = shape["offset"]["x"];
            wpnShape.offsetY = shape["offset"]["y"];
            wpnShape.anchorX = shape["anchor"]["x"];
            wpnShape.anchorY = shape["anchor"]["y"];
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

        WeaponCache[jsonFile] = wpnData;
        return wpnData;
    }

    return it->second;
}

void Weapon::CreateWeapon(const WeaponData &wpnData, std::weak_ptr<b2World> world, const Avatar &avatar)
{
    if (auto w = world.lock())
    {
        m_torque = wpnData.torque;
        m_speed = wpnData.speed;

        for (const auto &shape : wpnData.shapes)
        {
            b2BodyDef bodyDef;
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = avatar.GetPosition();
            bodyDef.position.x += avatar.GetShaftLength() * 2.0f + shape.offsetX;
            bodyDef.position.y += shape.offsetY;
            bodyDef.fixedRotation = false;
            auto asset = std::make_shared<Asset>(w->CreateBody(&bodyDef), "weapon_head");

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

            if (shape.anchorType == "weld")
            {
                auto body = avatar.GetShaft();
                if (m_assets.size() > 0)
                {
                    body = m_assets.back()->GetBody();
                }

                b2Vec2 anchorPos = avatar.GetPosition();
                anchorPos.x += avatar.GetShaftLength() * 2.0f + shape.anchorX;
                anchorPos.y += shape.anchorY;

                b2WeldJointDef joint;
                joint.Initialize(body, asset->GetBody(), anchorPos);
                m_joints.emplace_back(w->CreateJoint(&joint));
            }

            m_assets.emplace_back(asset);
        }
    }
}
