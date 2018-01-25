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
    template<typename T>
    void Save() const
    {
        std::filebuf fb;
        fb.open("test.txt", std::ios::out);
        std::ostream os(&fb);
        for (auto const &entity : mEntities) {
            for (auto const &component : entity->mComponents) {
                os.write(component.second->GetComponentName(), std::strlen(component.second->GetComponentName()));
                component.second->Save(os);
            }
        }
    }

private:
    std::vector<std::unique_ptr<Entity>> mEntities;
};

template<typename... C>
void EntityManager::Any(typename std::common_type<std::function<void(Entity*, C* ...)>>::type view)
{
    for (auto& entity : mEntities)
    {
        if (entity->HasAnyComponent<C...>())
        {
            view(entity.get(), entity->template GetComponent<C>()...);
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
            view(entity.get(), entity->template GetComponent<C>()...);
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