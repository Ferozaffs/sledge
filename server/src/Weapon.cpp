#include "Weapon.h"
#include "Asset.h"

#include <assert.h>
#include <box2d/box2d.h>

using namespace Gameplay;

Weapon::Weapon() : m_speed(3.0f), m_torque(5000.0f)
{
}

Weapon::~Weapon()
{
    BreakJoints();
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

b2Body *Weapon::GetShaft() const
{
    assert(m_shaftAsset != nullptr && "Weapon not implemented");

    return m_shaftAsset->GetBody();
}

void Weapon::BreakJoints()
{
    while (m_joints.empty() == false)
    {
        m_shaftAsset->GetBody()->GetWorld()->DestroyJoint(m_joints.back());
        m_joints.pop_back();
    };
}
