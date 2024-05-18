#include "Weapon.h"
#include "Asset.h"

#include <assert.h>
#include <box2d/box2d.h>

using namespace Gameplay;

Weapon::Weapon(const Avatar *avatar) : m_speed(3.0f), m_torque(5000.0f), m_avatar(avatar)
{
}

Weapon::~Weapon()
{
    BreakJoints();

    if (GetShaft() != nullptr)
    {
        GetShaft()->GetWorld()->DestroyBody(GetShaft());
    }
}

const float &Gameplay::Weapon::GetSpeed() const
{
    return m_speed;
}

const float &Gameplay::Weapon::GetTorque() const
{
    return m_torque;
}

b2Body *Gameplay::Weapon::GetShaft() const
{
    assert(m_shaftAsset != nullptr && "Weapon not implemented");

    return m_shaftAsset->GetBody();
}

const Avatar *Gameplay::Weapon::GetAvatar() const
{
    return m_avatar;
}

std::vector<std::shared_ptr<Asset>> Gameplay::Weapon::GetAssets() const
{
    return m_assets;
}

void Weapon::BreakJoints()
{
    while (m_joints.empty() == false)
    {
        GetShaft()->GetWorld()->DestroyJoint(m_joints.back());
        m_joints.pop_back();
    };
}
