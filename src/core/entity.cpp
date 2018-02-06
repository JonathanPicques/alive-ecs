#include "core/entity.hpp"
#include "core/entitymanager.hpp"

Entity::Entity(EntityManager* manager, PointerSize index, PointerSize version) : mManager(manager), mIndex(index), mVersion(version)
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