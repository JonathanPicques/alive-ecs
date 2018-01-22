#include <ios>
#include <iostream>
#include <core/entitymanager.hpp>
#include "core/entity.hpp"

class PhysicsComponent : public Component
{
public:
    DECLARE_COMPONENT(PhysicsComponent)
public:
    float xSpeed = 32;
    float ySpeed = 64;
};

class TransformComponent : public Component
{
public:
    DECLARE_COMPONENT(TransformComponent)
public:
    float x = 0;
    float y = 0;
};

DEFINE_COMPONENT(PhysicsComponent);
DEFINE_COMPONENT(TransformComponent);

int main()
{
    EntityManager manager;
    Entity &entity = *manager.Create();

    entity.AddComponent<Component>();
    entity.AddComponent<PhysicsComponent>();
    entity.AddComponent<TransformComponent>();

    auto physics = entity.GetComponent<PhysicsComponent>();

    std::cout << std::boolalpha << entity.HasComponents<Component, PhysicsComponent>() << std::endl;

    physics->xSpeed = 32;

    manager.With<TransformComponent, PhysicsComponent>([](auto e, auto component, auto physics) {
        std::cout << physics->xSpeed << std::endl;
    });

    entity.RemoveComponent<PhysicsComponent>();

    manager.With<TransformComponent, PhysicsComponent>([](auto e, auto component, auto physics) {
        std::cout << physics->xSpeed << std::endl;
    });

    entity.With<TransformComponent>([](auto transform) {

    });

    auto transforms = manager.With<TransformComponent>();

    return 0;
}