#include <ostream>
#include <istream>

#include <core/entitymanager.hpp>

#include "components.hpp"

DEFINE_COMPONENT(DummyComponent);
DEFINE_COMPONENT(PhysicsComponent);
DEFINE_COMPONENT(TransformComponent);

void TransformComponent::Serialize(std::ostream& out) const
{

    static_assert(std::is_pod<decltype(TransformComponent::mData)>::value, "TransformComponent is not POD");
    out.write((char*) &mData, sizeof(mData));
}

void TransformComponent::Deserialize(std::istream& is)
{
    static_assert(std::is_pod<decltype(TransformComponent::mData)>::value, "TransformComponent is not POD");
    is.read((char*) &mData, sizeof(mData));
}

float TransformComponent::GetX() const
{

    return mData.x;
}

float TransformComponent::GetY() const
{
    return mData.y;
}

std::unique_ptr<EntityManager> CreateEntityManager()
{
    auto manager = std::make_unique<EntityManager>();
    manager->RegisterComponent<DummyComponent>();
    manager->RegisterComponent<PhysicsComponent>();
    manager->RegisterComponent<TransformComponent>();
    return std::move(manager);
}