#include <iostream>
#include "Entity.hpp"

EntityHandle::EntityHandle(uint32_t mIndex, uint32_t mInstance) : mIndex(mIndex), mInstance(mInstance)
{

}

Entity::Entity(const EntityHandle& handle, EntityManager* manager) : mHandle(handle), mManager(manager)
{

}

bool Entity::Destroyed() const
{
    return !mManager->HandleDestroyed(mHandle);
}

EntityHandle EntityManager::NextHandle()
{
    std::uint32_t nextIndex = 0;
    std::uint32_t nextInstance = 0;
    if (!mFreeIndexes.empty())
    {
        nextIndex = mFreeIndexes.back();
        mFreeIndexes.pop_back();
    }
    else
    {
        nextIndex = mNextIndex++;
    }
    if (mComponentLists.size() <= nextIndex)
    {
        nextInstance = 0;
        mInstances.resize(nextIndex + 1);
        mComponentLists.resize(nextIndex + 1);
    }
    else
    {
        nextInstance = mInstances[nextIndex]++;
    }
    return {nextIndex, nextInstance};
}

Entity EntityManager::Create()
{
    return {NextHandle(), this};
}

void EntityManager::Destroy(EntityHandle handle)
{
    // TODO: Clear components
    mInstances[handle.mIndex] += 1;
    mFreeIndexes.emplace_back(handle.mIndex);
}

bool EntityManager::HandleDestroyed(EntityHandle handle) const
{
    if (handle.mIndex > mComponentLists.size())
    {
        return false;
    }
    return mInstances[handle.mIndex] != handle.mInstance;
}