//
// Created by John on 25/01/2018.
//

#include <core/entitymanager.hpp>

#include "components.hpp"

DEFINE_COMPONENT(DummyComponent);
DEFINE_COMPONENT(PhysicsComponent);
DEFINE_COMPONENT(TransformComponent);

std::unique_ptr<EntityManager> CreateEntityManager()
{
    auto manager = std::make_unique<EntityManager>();
    manager->RegisterComponent<DummyComponent>();
    manager->RegisterComponent<PhysicsComponent>();
    manager->RegisterComponent<TransformComponent>();
    return std::move(manager);
}