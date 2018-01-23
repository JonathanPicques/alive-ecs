#include "core/entity.hpp"
#include "core/entitymanager.hpp"

Entity::Entity(EntityManager* manager) : mManager(manager)
{

}

void Entity::Destroy()
{
    mManager->Destroy(this);
}

bool Entity::IsDestroyed() const
{
    return mDestroyed;
}