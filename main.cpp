#include <ios>
#include <cassert>
#include <iostream>

#include "core/entitymanager.hpp"

class TransformComponent : public Component
{
public:
    DECLARE_COMPONENT(TransformComponent);
public:
    float x = 0;
    float y = 0;
};

class PhysicsComponent : public Component
{
public:
    DECLARE_COMPONENT(PhysicsComponent);
public:
    float xSpeed = 0;
    float ySpeed = 0;
};

DEFINE_COMPONENT(TransformComponent);
DEFINE_COMPONENT(PhysicsComponent);

int main()
{
    EntityManager manager;
    Entity &entity = *manager.Create();

    entity.AddComponent<PhysicsComponent>();
    entity.AddComponent<TransformComponent>();

    entity.GetComponent<PhysicsComponent>()->xSpeed = 10;
    entity.GetComponent<PhysicsComponent>()->ySpeed = 1;

    for (auto i = 0; i < 10; i++) {
        // PhysicsSystem
        manager.With<PhysicsComponent, TransformComponent>([](auto entity, auto physics, auto transform) {
            transform->x += physics->xSpeed;
            transform->y += physics->ySpeed;
        });
    }

    entity.With<TransformComponent>([](auto transform) {
        assert(transform->x == 100);
        assert(transform->y == 10);
        std::cout << "Well done" << std::endl;
    });

    return 0;
}