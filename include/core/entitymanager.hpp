#include <memory>
#include <vector>
#include <functional>

#include "entity.hpp"

class EntityManager final
{
public:
    Entity *Create();
    void Destroy(Entity *entity);

public:
    template<typename ...C>
    void With(typename std::common_type<std::function<void(Entity *, C *...)>>::type view);

private:
    std::vector<std::unique_ptr<Entity>> mEntities;
};

template<typename ...C>
void EntityManager::With(typename std::common_type<std::function<void(Entity *, C *...)>>::type view)
{
    for (auto &entity : mEntities) {
        if (entity->HasComponents<C...>()) {
            view(entity.get(), entity->template GetComponent<C>()...);
        }
    }
}