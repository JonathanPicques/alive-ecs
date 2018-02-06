#include "core/entity.hpp"
#include "core/entitymanager.hpp"

Entity::Entity(PointerSize index, PointerSize version, EntityManager* manager) : mIndex(index), mVersion(version), mManager(manager)
{

}

bool operator==(const Entity& a, const Entity& b)
{
    return a.mIndex == b.mIndex && a.mVersion == b.mVersion;
}

void Entity::Destroy()
{
    mManager->DestroyEntity(*this);
}

bool Entity::IsDestroyed() const
{
    return !mManager->EntityPointerValid(*this);
}

void Entity::ResolveComponentDependencies()
{
    mManager->EntityResolveComponentDependencies(*this);
}

EntityManager* Entity::GetManager()
{
    return mManager;
}