#pragma once

#include <memory>

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
    void Serialize(std::ostream& out) const override
    {
        static_assert(std::is_pod<decltype(TransformComponent::mData)>::value);
        out.write((char*) &mData, sizeof(mData));
    }
    void Deserialize(std::istream& is) override
    {
        static_assert(std::is_pod<decltype(TransformComponent::mData)>::value);
        is.read((char*) &mData, sizeof(mData));
    }

public:
    float GetX() const
    { return mData.x; }
    float GetY() const
    { return mData.y; }

public:
    struct
    {
        float x;
        float y;
    } mData = {};
};

std::unique_ptr<EntityManager> CreateEntityManager();