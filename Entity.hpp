#pragma once

#include <map>
#include <memory>
#include <vector>
#include <cstdint>
#include <utility>
#include <stdexcept>

class EntityManager;

using type_id_t = void (*)();

template<typename>
void type_id()
{}

class EntityHandle final
{
public:
    EntityHandle(std::uint32_t mIndex, std::uint32_t mInstance);
public:
    std::uint32_t mIndex = 0;
    std::uint32_t mInstance = 0;
};

template<typename C>
class ComponentHandle final
{
public:
    ComponentHandle(EntityHandle handle, EntityManager* entityManager);
public:
    bool Valid() const;
public:
    C* get();
public:
    EntityHandle mEntityHandle;
    EntityManager* mEntityManager = nullptr;
};

class Entity
{
public:
    Entity(const EntityHandle& handle, EntityManager* manager);
public:
    bool Valid() const;
public:
    template<typename C, typename ...Args>
    ComponentHandle<C> AddComponent(Args&& ...args);
    template<typename C>
    ComponentHandle<C> GetComponent();
public:
    EntityHandle mHandle;
    EntityManager* mManager = nullptr;
};

class BaseComponentContainer
{
public:
    virtual ~BaseComponentContainer() = default;
};

template<typename T>
class ComponentContainer : public BaseComponentContainer
{
public:
    explicit ComponentContainer(const T& data);
public:
    T mData;
};

class ComponentList
{
public:
    std::map<type_id_t, std::unique_ptr<BaseComponentContainer>> mComponents;
};

class EntityManager final
{
public:
    Entity Create();
    void Destroy(class EntityHandle handle);
public:
    EntityHandle NextHandle();
    bool HandleValid(class EntityHandle handle) const;
public:
    template<typename C, typename ...Args>
    ComponentHandle<C> AddComponent(EntityHandle handle, Args&& ...args);
    template<typename C>
    ComponentHandle<C> GetComponent(EntityHandle handle);
public:
    std::uint32_t mNextIndex = 0;
    std::vector<std::uint32_t> mInstances;
    std::vector<std::uint32_t> mFreeIndexes;
    std::vector<ComponentList> mComponentLists;
};

template<typename C>
ComponentHandle<C>::ComponentHandle(EntityHandle handle, EntityManager* entityManager)
    : mEntityHandle(handle), mEntityManager(entityManager)
{

}

template<typename C>
C* ComponentHandle<C>::get()
{
    if (mEntityManager->HandleValid(mEntityHandle))
    {
        throw new std::logic_error("invalid entity handle");
    }
    auto &components = mEntityManager->mComponentLists[mEntityHandle.mIndex].mComponents;
    auto componentContainerFound = components.find(type_id<C>);
    if (componentContainerFound != components.end()) {
       return &(static_cast<ComponentContainer<C> *>(componentContainerFound->second.get()))->mData;
    }
    return nullptr;
}

template<typename C>
bool ComponentHandle<C>::Valid() const {
    if (mEntityManager->HandleValid(mEntityHandle))
    {
        return false;
    }
    auto &components = mEntityManager->mComponentLists[mEntityHandle.mIndex].mComponents;
    auto componentContainerFound = components.find(type_id<C>);
    return componentContainerFound != components.end();
}

template<typename C, typename... Args>
ComponentHandle<C> Entity::AddComponent(Args&& ...args)
{
    return mManager->AddComponent<C>(mHandle, std::forward<Args>(args)...);
}

template<typename C>
ComponentHandle<C> Entity::GetComponent()
{
    return mManager->GetComponent<C>(mHandle);
}

template<typename C, typename... Args>
ComponentHandle<C> EntityManager::AddComponent(EntityHandle handle, Args&& ...args)
{
    if (HandleValid(handle))
    {
        throw new std::logic_error("invalid entity handle");
    }
    mComponentLists[handle.mIndex].mComponents[type_id<C>] = std::make_unique<ComponentContainer<C>>(C(std::forward<Args>(args)...));
    return {handle, this};
}

template<typename C>
ComponentHandle<C> EntityManager::GetComponent(EntityHandle handle)
{
    if (HandleValid(handle))
    {
        throw new std::logic_error("invalid entity handle");
    }
    return {handle, this};
}

template<typename T>
ComponentContainer<T>::ComponentContainer(const T& data) : mData(data)
{

}