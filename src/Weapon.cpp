#include "Weapon.h"

#include <box2d/box2d.h>
#include <assert.h>

using namespace Gameplay;

Weapon::Weapon(const Avatar* avatar)
	: m_shaft(nullptr)
	, m_avatar(avatar)
	, m_speed(3.0f)
	, m_torque(5000.0f)
{

}

Weapon::~Weapon()
{
	if (m_shaft != nullptr)
	{
		m_shaft->GetWorld()->DestroyBody(m_shaft);
	}
}

const float& Gameplay::Weapon::GetSpeed() const
{
	return m_speed;
}

const float& Gameplay::Weapon::GetTorque() const
{
	return m_torque;
}

b2Body* Gameplay::Weapon::GetShaft() const
{
	assert(m_shaft != nullptr && "Weapon not implemented");

	return m_shaft;
}

const Avatar* Gameplay::Weapon::GetAvatar() const
{
	return m_avatar;
}
