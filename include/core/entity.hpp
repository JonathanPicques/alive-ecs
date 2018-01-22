#pragma once

#include <map>
#include <memory>

#include "component.hpp"

class EntityManager;

class Entity final
{
public:
    explicit Entity(EntityManager *manager);

public:
    Entity(Entity &&) = delete;
    Entity(Entity const&) = delete;
    Entity &operator=(Entity const&) = delete;

public:
    template<typename C>
    bool HasComponents();
    template<typename C1, typename C2, typename ...C>
    bool HasComponents();
    template<typename C>
    C *GetComponent();
    template<typename C, typename ...Args>
    C *AddComponent(Args &&...args);
    template<typename C>
    void RemoveComponent();

public:
    void Destroy();

private:
    EntityManager *mManager;
    std::map<const char *, std::unique_ptr<Component>> mComponents;
};

template<typename C>
bool Entity::HasComponents()
{
    return mComponents.find(C::ComponentName) != mComponents.end();
}

template<typename C1, typename C2, typename ...C>
bool Entity::HasComponents()
{
    return HasComponents<C1>() && HasComponents<C2, C...>();
}

template<typename C, typename ...Args>
C *Entity::AddComponent(Args &&...args)
{
    auto component = std::make_unique<C>(std::forward<Args>(args)...);
    auto componentPtr = component.get();
    componentPtr->mEntity = this;
    mComponents[C::ComponentName] = std::move(component);
    return componentPtr;
}

template<typename C>
C *Entity::GetComponent()
{
    auto found = mComponents.find(C::ComponentName);
    if (found != mComponents.end())
    {
        return static_cast<C *>(found->second.get());
    }
    return nullptr;
}

template<typename C>
void Entity::RemoveComponent()
{
    auto found = mComponents.find(C::ComponentName);
    if (found != mComponents.end())
    {
        mComponents.erase(found);
    }
}