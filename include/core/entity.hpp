#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <algorithm>

#include "component.hpp"

class EntityManager;

class Entity final
{
public:
    friend EntityManager;

public:
    explicit Entity(EntityManager* manager);

public:
    Entity(Entity&&) = delete;
    Entity(Entity const&) = delete;
    Entity& operator=(Entity const&) = delete;

public:
    EntityManager* GetManager();

public:
    template<typename C>
    C* GetComponent();
    template<typename C, typename ...Args>
    C* AddComponent(Args&& ...args);
    template<typename C>
    void RemoveComponent();

public:
    template<typename C>
    bool HasComponent() const;
    template<typename C1, typename C2, typename ...C>
    bool HasComponent() const;
    template<typename C>
    bool HasAnyComponent() const;
    template<typename C1, typename C2, typename ...C>
    bool HasAnyComponent() const;

public:
    template<typename ...C>
    bool Any(typename std::common_type<std::function<void(C* ...)>>::type view);
    template<typename ...C>
    bool With(typename std::common_type<std::function<void(C* ...)>>::type view);

public:
    void Destroy();
    bool IsDestroyed() const;

private:
    void ConstructComponent(Component& component);

private:
#if defined(_DEBUG)
    void AssertComponentRegistered(const char* componentName) const;
#endif

private:
    bool mDestroyed = false;
    EntityManager* mManager = nullptr;
    std::vector<std::unique_ptr<Component>> mComponents;
};

template<typename C>
C* Entity::GetComponent()
{
    auto found = std::find_if(mComponents.begin(), mComponents.end(), [](auto& c)
    {
        return std::string{ C::ComponentName } == std::string{ c->GetComponentName() };
    });
    if (found != mComponents.end())
    {
        return static_cast<C*>(found->get());
    }
    return nullptr;
}

template<typename C, typename ...Args>
C* Entity::AddComponent(Args&& ...args)
{
#if defined(_DEBUG)
    AssertComponentRegistered(C::ComponentName);
#endif
    if (HasComponent<C>())
    {
        throw std::logic_error(std::string{ "Entity::AddComponent: Component " } + C::ComponentName + std::string{ " already exists" });
    }
    auto component = std::make_unique<C>(std::forward<Args>(args)...);
    auto componentPtr = component.get();
    mComponents.emplace_back(std::move(component));
    ConstructComponent(*componentPtr);
    return componentPtr;
}

template<typename C>
void Entity::RemoveComponent()
{
#if defined(_DEBUG)
    AssertComponentRegistered(C::ComponentName);
#endif
    if (!HasComponent<C>())
    {
        throw std::logic_error(std::string{ "Entity::RemoveComponent: Component " } + C::ComponentName + std::string{ " not found" });
    }
    auto found = std::find_if(mComponents.begin(), mComponents.end(), [](auto& c)
    {
        return std::string{ C::ComponentName } == std::string{ c->GetComponentName() };
    });
    if (found != mComponents.end())
    {
        mComponents.erase(found);
    }
}

template<typename C>
bool Entity::HasComponent() const
{
    return std::find_if(mComponents.begin(), mComponents.end(), [](auto& c)
    {
        return std::string{ C::ComponentName } == std::string{ c->GetComponentName() };
    }) != mComponents.end();
}

template<typename C1, typename C2, typename ...C>
bool Entity::HasComponent() const
{
    return HasComponent<C1>() && HasComponent<C2, C...>();
}

template<typename C>
bool Entity::HasAnyComponent() const
{
    return std::find_if(mComponents.begin(), mComponents.end(), [](auto& c)
    {
        return std::string{ C::ComponentName } == std::string{ c->GetComponentName() };
    }) != mComponents.end();
}

template<typename C1, typename C2, typename ...C>
bool Entity::HasAnyComponent() const
{
    if (HasComponent<C1>())
    {
        return true;
    }
    return HasComponent<C2, C...>();
}

template<typename ...C>
bool Entity::Any(typename std::common_type<std::function<void(C* ...)>>::type view)
{
    if (HasAnyComponent<C...>())
    {
        view(GetComponent<C>()...);
        return true;
    }
    return false;
}

template<typename ...C>
bool Entity::With(typename std::common_type<std::function<void(C* ...)>>::type view)
{
    if (HasComponent<C...>())
    {
        view(GetComponent<C>()...);
        return true;
    }
    return false;
}