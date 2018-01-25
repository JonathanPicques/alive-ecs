#include <gtest/gtest.h>

#include <core/entitymanager.hpp>

#include "test_components/components.hpp"

TEST(EntityManager, EntityManager_Save)
{
    EntityManager manager;
    auto entity = manager.Create();
    entity->AddComponent<DummyComponent>();
    entity->AddComponent<PhysicsComponent>();
    entity->AddComponent<TransformComponent>(32.0f, 64.0f);
    manager.Save<TransformComponent *>();
}