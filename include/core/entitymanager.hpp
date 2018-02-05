#pragma once

#include <memory>
#include <vector>
#include <string>
#include <iosfwd>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <unordered_map>
#include <bits/unordered_map.h>

#include "system.hpp"
#include "entity.hpp"

class EntityManager final
{
public:
    friend Entity;

public:
    Entity CreateEntity();
    template<typename ...C>
    Entity CreateEntityWith();

private:
    template<typename C>
    void CreateEntityWith(Entity& entity);
    template<typename C1, typename C2, typename ...C>
    void CreateEntityWith(Entity& entity);

private:
    void DestroyEntity(Entity::Pointer& entityPointer);

public:
    template<typename S, typename ...Args>
    S* AddSystem(Args&& ...args);
    template<typename S>
    S* GetSystem();
    template<typename S>
    const S* GetSystem() const;
    template<typename S>
    void RemoveSystem();
    template<typename S>
    bool HasSystem();

public:
    template<typename C>
    void RegisterComponent();
#if defined(_DEBUG)
    bool IsComponentRegistered(const std::string& componentName) const;
    void AssertComponentRegistered(const std::string& componentName) const;
#endif

public:
    template<typename ...C>
    void Any(typename std::common_type<std::function<void(Entity entity, C* ...)>>::type view);
    template<typename ...C>
    std::vector<Entity> Any();
    template<typename ...C>
    void With(typename std::common_type<std::function<void(Entity entity, C* ...)>>::type view);
    template<typename ...C>
    std::vector<Entity> With();

public:
    void Serialize(std::ostream& os) const;
    void Deserialize(std::istream& is);

private:
    bool EntityPointerValid(const Entity::Pointer& entityPointer) const;
    void AssertEntityPointerValid(const Entity::Pointer& entityPointer) const;

private:
    template<typename C>
    C* EntityGetComponent(const Entity::Pointer& entityPointer);
    template<typename C>
    const C* EntityGetComponent(const Entity::Pointer& entityPointer) const;
    template<typename C>
    C* EntityAddComponent(const Entity::Pointer& entityPointer);
    template<typename C>
    void EntityRemoveComponent(const Entity::Pointer& entityPointer);

private:
    void EntityConstructComponent(Component* component, const Entity::Pointer& entityPointer);
    void EntityResolveComponentDependencies(const Entity::Pointer& entityPointer);

private:
    template<typename C>
    bool EntityHasComponent(const Entity::Pointer& entityPointer) const;
    template<typename C1, typename C2, typename ...C>
    bool EntityHasComponent(const Entity::Pointer& entityPointer) const;
    template<typename C>
    bool EntityHasAnyComponent(const Entity::Pointer& entityPointer) const;
    template<typename C1, typename C2, typename ...C>
    bool EntityHasAnyComponent(const Entity::Pointer& entityPointer) const;

private:
    template<typename ...C>
    bool EntityAny(const Entity::Pointer& entityPointer, typename std::common_type<std::function<void(C* ...)>>::type view);
    template<typename ...C>
    bool EntityWith(const Entity::Pointer& entityPointer, typename std::common_type<std::function<void(C* ...)>>::type view);

private:
    struct EntityComponentContainer final
    {
        bool mCreated = false;
        std::vector<std::unique_ptr<Component>> mComponents = {};
    };

private:
    std::uint16_t mIndex = {};
    std::vector<std::uint16_t> mVersions = {};
    std::vector<std::uint16_t> mFreeIndexes = {};
    std::vector<std::unique_ptr<System>> mSystems = {};
    std::vector<EntityComponentContainer> mEntityComponents = {};
    std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>> mRegisteredComponents = {};
};

template<typename C>
C* Entity::GetComponent()
{
    return mManager->EntityGetComponent<C>(mPointer);
}

template<typename C>
const C* Entity::GetComponent() const
{
    return mManager->EntityGetComponent<C>(mPointer);
}

template<typename C>
C* Entity::AddComponent()
{
    return mManager->EntityAddComponent<C>(mPointer);
}

template<typename C>
void Entity::RemoveComponent()
{
    mManager->EntityRemoveComponent<C>(mPointer);
}

template<typename C>
bool Entity::HasComponent() const
{
    return mManager->EntityHasComponent<C>(mPointer);
}

template<typename C1, typename C2, typename ...C>
bool Entity::HasComponent() const
{
    return mManager->EntityHasComponent<C1, C2, C...>(mPointer);
}

template<typename C>
bool Entity::HasAnyComponent() const
{
    return mManager->EntityHasAnyComponent<C>(mPointer);
}

template<typename C1, typename C2, typename ...C>
bool Entity::HasAnyComponent() const
{
    return mManager->EntityHasAnyComponent<C1, C2, C...>(mPointer);
}

template<typename... C>
bool Entity::Any(typename std::common_type<std::function<void(C* ...)>>::type view)
{
    return mManager->EntityAny(mPointer, view);
}

template<typename... C>
bool Entity::With(typename std::common_type<std::function<void(C* ...)>>::type view)
{
    return mManager->EntityWith(mPointer, view);
}

template<typename... C>
Entity EntityManager::CreateEntityWith()
{
    auto entity = CreateEntity();
    CreateEntityWith<C...>(entity);
    EntityResolveComponentDependencies(entity.mPointer);
    return entity;
}

template<typename C>
void EntityManager::CreateEntityWith(Entity& entity)
{
    entity.AddComponent<C>();
}

template<typename C1, typename C2, typename ...C>
void EntityManager::CreateEntityWith(Entity& entity)
{
    CreateEntityWith<C1>(entity);
    CreateEntityWith<C2, C...>(entity);
}

template<typename S, typename... Args>
S* EntityManager::AddSystem(Args&& ... args)
{
    if (HasSystem<S>())
    {
        throw std::logic_error(std::string{ "EntityManager::AddSystem: System " } + S::SystemName + std::string{ " already exists" });
    }
    auto system = std::make_unique<S>(std::forward<Args>(args)...);
    auto systemPtr = system.get();
    mSystems.emplace_back(std::move(system));
    systemPtr->mManager = this;
    return systemPtr;
}

template<typename S>
S* EntityManager::GetSystem()
{
    return const_cast<S*>(static_cast<const EntityManager*>(this)->GetSystem<S>());
}

template<typename S>
const S* EntityManager::GetSystem() const
{
    auto found = std::find_if(mSystems.begin(), mSystems.end(), [](const auto& s)
    {
        return s->GetSystemName() == S::SystemName;
    });
    if (found != mSystems.end())
    {
        return static_cast<S*>(found->get());
    }
    return nullptr;
}

template<typename S>
void EntityManager::RemoveSystem()
{
    if (!HasSystem<S>())
    {
        throw std::logic_error(std::string{ "EntityManager::RemoveSystem: System " } + S::SystemName + std::string{ " not found" });
    }
    auto found = std::find_if(mSystems.begin(), mSystems.end(), [](const auto& s)
    {
        return s->GetSystemName() == S::SystemName;
    });
    if (found != mSystems.end())
    {
        mSystems.erase(found);
    }
}

template<typename S>
bool EntityManager::HasSystem()
{
    return GetSystem<S>() != nullptr;
}

template<typename C>
void EntityManager::RegisterComponent()
{
    mRegisteredComponents[C::ComponentName] = []()
    {
        return std::make_unique<C>();
    };
}

template<typename C>
C* EntityManager::EntityGetComponent(const Entity::Pointer& entityPointer)
{
    AssertEntityPointerValid(entityPointer);
    return const_cast<C*>(static_cast<const EntityManager*>(this)->EntityGetComponent<C>(entityPointer));
}

template<typename C>
const C* EntityManager::EntityGetComponent(const Entity::Pointer& entityPointer) const
{
    AssertEntityPointerValid(entityPointer);
    auto& components = mEntityComponents[entityPointer.mIndex].mComponents;
    auto found = std::find_if(components.begin(), components.end(), [](const auto& c)
    {
        return c->GetComponentName() == C::ComponentName;
    });
    if (found != components.end())
    {
        return static_cast<C*>(found->get());
    }
    return nullptr;
}

template<typename C>
C* EntityManager::EntityAddComponent(const Entity::Pointer& entityPointer)
{
    AssertEntityPointerValid(entityPointer);
#if defined(_DEBUG)
    AssertComponentRegistered(C::ComponentName);
#endif
    if (EntityHasComponent<C>(entityPointer))
    {
        throw std::logic_error(std::string{ "Entity::AddComponent: Component " } + C::ComponentName + std::string{ " already exists" });
    }
    auto component = std::make_unique<C>();
    auto componentPtr = component.get();
    mEntityComponents[entityPointer.mIndex].mComponents.emplace_back(std::move(component));
    EntityConstructComponent(componentPtr, entityPointer);
    return componentPtr;

}

template<typename C>
void EntityManager::EntityRemoveComponent(const Entity::Pointer& entityPointer)
{
    AssertEntityPointerValid(entityPointer);
#if defined(_DEBUG)
    AssertComponentRegistered(C::ComponentName);
#endif
    auto& components = mEntityComponents[entityPointer.mIndex].mComponents;
    if (!EntityHasComponent<C>(entityPointer))
    {
        throw std::logic_error(std::string{ "Entity::RemoveComponent: Component " } + C::ComponentName + std::string{ " not found" });
    }
    auto found = std::find_if(components.begin(), components.end(), [](const auto& c)
    {
        return C::ComponentName == c->GetComponentName();
    });
    if (found != components.end())
    {
        components.erase(found);
    }
}

template<typename C>
bool EntityManager::EntityHasComponent(const Entity::Pointer& entityPointer) const
{
    AssertEntityPointerValid(entityPointer);
    return EntityGetComponent<C>(entityPointer) != nullptr;
}

template<typename C1, typename C2, typename ...C>
bool EntityManager::EntityHasComponent(const Entity::Pointer& entityPointer) const
{
    AssertEntityPointerValid(entityPointer);
    return EntityHasComponent<C1>(entityPointer) && EntityHasComponent<C2, C...>(entityPointer);
}

template<typename C>
bool EntityManager::EntityHasAnyComponent(const Entity::Pointer& entityPointer) const
{
    AssertEntityPointerValid(entityPointer);
    return EntityGetComponent<C>(entityPointer) != nullptr;
}

template<typename C1, typename C2, typename ...C>
bool EntityManager::EntityHasAnyComponent(const Entity::Pointer& entityPointer) const
{
    AssertEntityPointerValid(entityPointer);
    if (EntityHasComponent<C1>(entityPointer))
    {
        return true;
    }
    return EntityHasComponent<C2, C...>(entityPointer);
}

template<typename... C>
bool EntityManager::EntityAny(const Entity::Pointer& entityPointer, typename std::common_type<std::function<void(C* ...)>>::type view)
{
    AssertEntityPointerValid(entityPointer);
    if (EntityHasAnyComponent<C...>(entityPointer))
    {
        view(EntityGetComponent<C>(entityPointer)...);
        return true;
    }
    return false;
}

template<typename... C>
bool EntityManager::EntityWith(const Entity::Pointer& entityPointer, typename std::common_type<std::function<void(C* ...)>>::type view)
{
    AssertEntityPointerValid(entityPointer);
    if (EntityHasComponent<C...>(entityPointer))
    {
        view(EntityGetComponent<C>(entityPointer)...);
        return true;
    }
    return false;
}

template<typename... C>
void EntityManager::Any(typename std::common_type<std::function<void(Entity entity, C* ...)>>::type view)
{
    for (std::uint16_t i = 0; i < mEntityComponents.size(); ++i)
    {
        auto entity = Entity(this, Entity::Pointer(i, mVersions[i]));
        if (entity.HasAnyComponent<C...>())
        {
            view(entity, entity.GetComponent<C>()...);
        }
    }
}

template<typename... C>
std::vector<Entity> EntityManager::Any()
{
    std::vector<Entity> entities;
    for (std::uint16_t i = 0; i < mEntityComponents.size(); ++i)
    {
        auto entity = Entity(this, Entity::Pointer(i, mVersions[i]));
        if (entity.HasAnyComponent<C...>())
        {
            entities.emplace_back(entity);
        }
    }
    return entities;
}

template<typename... C>
void EntityManager::With(typename std::common_type<std::function<void(Entity entity, C* ...)>>::type view)
{
    for (std::uint16_t i = 0; i < mEntityComponents.size(); ++i)
    {
        if (mEntityComponents[i].mCreated)
        {
            auto entity = Entity(this, Entity::Pointer(i, mVersions[i]));
            if (entity.HasComponent<C...>())
            {
                view(entity, entity.GetComponent<C>()...);
            }
        }
    }
}

template<typename... C>
std::vector<Entity> EntityManager::With()
{
    std::vector<Entity> entities;
    for (std::uint16_t i = 0; i < mEntityComponents.size(); ++i)
    {
        if (mEntityComponents[i].mCreated)
        {
            auto entity = Entity(this, Entity::Pointer(i, mVersions[i]));
            if (entity.HasComponent<C...>())
            {
                entities.emplace_back(entity);
            }
        }
    }
    return entities;
}