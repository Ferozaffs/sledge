#pragma once
#include "B2Manager.h"
#include <box2d/box2d.h>

#include "LevelBlock.h"

namespace Physics
{

class ContactListener : public b2ContactListener
{
  public:
    void BeginContact(b2Contact *contact) override
    {
        HandleContact(contact, true);
    }

    void EndContact(b2Contact *contact) override
    {
        HandleContact(contact, false);
    }

  private:
    void HandleContact(b2Contact *contact, bool isBegin)
    {
        b2Fixture *fixtureA = contact->GetFixtureA();
        b2Fixture *fixtureB = contact->GetFixtureB();

        b2Body *bodyA = fixtureA->GetBody();
        b2Body *bodyB = fixtureB->GetBody();

        auto *userDataRaw = reinterpret_cast<PhysicsObjectUserData *>(bodyA->GetUserData().pointer);
        if (userDataRaw && userDataRaw->type == BodyType::LevelBlock)
        {
            Gameplay::LevelBlock *block = static_cast<Gameplay::LevelBlock *>(userDataRaw->object);
            block->OnContact(bodyB, fixtureB, isBegin);
        }
        userDataRaw = reinterpret_cast<PhysicsObjectUserData *>(bodyB->GetUserData().pointer);
        if (userDataRaw && userDataRaw->type == BodyType::LevelBlock)
        {
            Gameplay::LevelBlock *block = static_cast<Gameplay::LevelBlock *>(userDataRaw->object);
            block->OnContact(bodyB, fixtureB, isBegin);
        }
    }
};
} // namespace Physics
