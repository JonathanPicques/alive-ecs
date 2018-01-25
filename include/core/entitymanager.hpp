#include <memory>
#include <vector>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <functional>

#include "entity.hpp"

class EntityManager final
{
public:
    Entity* Create();
    void Destroy(Entity* entity);
    void DestroyEntities();

public:
    template<typename ...C>
    void Any(typename std::common_type<std::function<void(Entity*, C* ...)>>::type view);
    template<typename ...C>
    std::vector<Entity*> Any();
    template<typename ...C>
    void With(typename std::common_type<std::function<void(Entity*, C* ...)>>::type view);
    template<typename ...C>
    std::vector<Entity*> With();

public:
    template<typename C>
    void RegisterComponent();

public:
    void Save(std::ostream &os) const;
    void Load(std::istream &is);

private:
    std::vector<std::unique_ptr<Entity>> mEntities;
    std::map<const char*, std::function<std::unique_ptr<Component>()>> mRegisteredComponents;
};

template<typename... C>
void EntityManager::Any(typename std::common_type<std::function<void(Entity*, C* ...)>>::type view)
{
    for (auto& entity : mEntities)
    {
        if (entity->HasAnyComponent<C...>())
        {
            view(entity.get(), entity->GetComponent<C>()...);
        }
    }
}

template<typename... C>
std::vector<Entity*> EntityManager::Any()
{
    std::vector<Entity*> entities;
    for (auto& entity : mEntities)
    {
        if (entity->HasAnyComponent<C...>())
        {
            entities.emplace_back(entity.get());
        }
    }
    return entities;
}

template<typename ...C>
void EntityManager::With(typename std::common_type<std::function<void(Entity*, C* ...)>>::type view)
{
    for (auto& entity : mEntities)
    {
        if (entity->HasComponent<C...>())
        {
            view(entity.get(), entity->GetComponent<C>()...);
        }
    }
}

template<typename ...C>
std::vector<Entity*> EntityManager::With()
{
    std::vector<Entity*> entities;
    for (auto& entity : mEntities)
    {
        if (entity->HasComponent<C...>())
        {
            entities.emplace_back(entity.get());
        }
    }
    return entities;
}

template<typename C>
void EntityManager::RegisterComponent()
{
    mRegisteredComponents[C::ComponentName] = []()
    {
        return std::make_unique<C>();
    };
}

// TODO: Bleurk, should be in Entity.hpp, but ... forward hell

template<typename C, typename ...Args>
C* Entity::AddComponent(Args&& ...args)
{
    mManager->RegisterComponent<C>();
    auto component = std::make_unique<C>(std::forward<Args>(args)...);
    auto componentPtr = component.get();
    componentPtr->mEntity = this;
    mComponents[C::ComponentName] = std::move(component);
    return componentPtr;
}