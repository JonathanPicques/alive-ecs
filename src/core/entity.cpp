#include "core/entity.hpp"
#include "core/entitymanager.hpp"

Entity::Entity(PointerSize index, PointerSize version, EntityManager* manager) : mIndex(index), mVersion(version), mManager(manager)
{

}

bool Entity::IsValid() const
{
    return mManager->IsEntityPointerValid(*this);
}

Entity::operator bool() const
{
    return IsValid();
}

void Entity::Destroy()
{
    mManager->DestroyEntity(*this);
}

void Entity::ResolveComponentDependencies()
{
    mManager->EntityResolveComponentDependencies(*this);
}

EntityManager* Entity::GetManager()
{
    return mManager;
}

bool operator==(const Entity& a, const Entity& b)
{
    return a.mIndex == b.mIndex && a.mVersion == b.mVersion;
}