#pragma once

#include <core/component.hpp>

class DummyComponent : public Component
{
public:
    DECLARE_COMPONENT(DummyComponent);
};

class PhysicsComponent : public Component
{
public:
    DECLARE_COMPONENT(PhysicsComponent);
};

class TransformComponent : public Component
{
public:
    DECLARE_COMPONENT(TransformComponent);

public:
    TransformComponent()
    {}
    TransformComponent(float x, float y) : mData{ x, y }
    {}

public:
    void Save(std::ostream &out) const override
    {
        static_assert(std::is_pod<decltype(TransformComponent::mData)>::value);
        out.write((char*) &mData, sizeof(mData));
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
    } mData;
};