#pragma once

#include <memory>
#include <iosfwd>

#include <core/component.hpp>

class EntityManager;

class DummyComponent final : public Component
{
public:
    DECLARE_COMPONENT(DummyComponent);
};

class PhysicsComponent final : public Component
{
public:
    DECLARE_COMPONENT(PhysicsComponent);
};

class TransformComponent final : public Component
{
public:
    DECLARE_COMPONENT(TransformComponent);

public:
    TransformComponent() = default;
    TransformComponent(float x, float y) : mData{ x, y }
    {}

public:
    void Serialize(std::ostream& out) const override;
    void Deserialize(std::istream& is) override;

public:
    float GetX() const;
    float GetY() const;

public:
    struct
    {
        float x;
        float y;
    } mData = {};
};

std::unique_ptr<EntityManager> CreateEntityManager();