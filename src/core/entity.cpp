#include "core/entity.hpp"
#include "core/entitymanager.hpp"

Entity::Pointer::Pointer(Entity::PointerSize index, Entity::PointerSize version) : mIndex(index), mVersion(version)
{

}

bool operator==(const Entity::Pointer& a, const Entity::Pointer& b)
{
    return a.mIndex == b.mIndex && a.mVersion == b.mVersion;
}

bool operator!=(const Entity::Pointer& a, const Entity::Pointer& b)
{
    return a.mIndex != b.mIndex || a.mVersion != b.mVersion;
}

Entity::Entity(EntityManager* manager, Pointer pointer) : mManager(manager), mPointer(pointer)
{

}

bool operator==(const Entity& a, const Entity& b)
{
    return a.mPointer == b.mPointer;
}

void Entity::Destroy()
{
    mManager->DestroyEntity(mPointer);
}

bool Entity::IsDestroyed() const
{
    return !mManager->EntityPointerValid(mPointer);
}

void Entity::ResolveComponentDependencies()
{
    mManager->EntityResolveComponentDependencies(mPointer);
}

EntityManager* Entity::GetManager()
{
    return mManager;
}

const Entity::Pointer& Entity::GetPointer() const
{
    return mPointer;
}
