#include <sstream>
#include <ostream>
#include <istream>
#include <cstring>
#include <algorithm>

#include "core/entitymanager.hpp"

Entity EntityManager::CreateEntity()
{
    std::uint16_t index;
    std::uint16_t version;
    if (mFreeIndexes.empty())
    {
        index = mIndex++;
        version = 0;
        mVersions.resize(index + 1);
        mEntityComponents.resize(index + 1);
        mEntityComponents[index].mCreated = true;
    }
    else
    {
        index = mFreeIndexes.back();
        version = mVersions[index];
        mFreeIndexes.pop_back();
    }
    return Entity(this, Entity::Pointer(index, version));
}

void EntityManager::DestroyEntity(Entity::Pointer& entityPointer)
{
    AssertEntityPointerValid(entityPointer);
    mVersions[entityPointer.mIndex] += 1;
    mEntityComponents[entityPointer.mIndex].mCreated = false;
    mEntityComponents[entityPointer.mIndex].mComponents.clear();
    mFreeIndexes.push_back(entityPointer.mIndex);
}

void EntityManager::Serialize(std::ostream& os) const
{
    throw std::logic_error("not yet implemented");
}

void EntityManager::Deserialize(std::istream& is)
{
    throw std::logic_error("not yet implemented");
}

bool EntityManager::EntityPointerValid(const Entity::Pointer& entityPointer) const
{
    return entityPointer.mIndex < mVersions.size() && mVersions[entityPointer.mIndex] == entityPointer.mVersion;
}

void EntityManager::AssertEntityPointerValid(const Entity::Pointer& entityPointer) const
{
    if (!EntityPointerValid(entityPointer))
    {
        std::stringstream errorFormat;
        errorFormat << "Entity::Pointer invalid: " << entityPointer.mIndex << "(" << entityPointer.mIndex << ")";
        throw std::logic_error(errorFormat.str());
    }
}

void EntityManager::EntityConstructComponent(Component* component, const Entity::Pointer& entityPointer)
{
    AssertEntityPointerValid(entityPointer);
    component->mEntity = std::make_unique<Entity>(this, entityPointer); // TODO: move from pointer to value
    component->OnLoad();
}

void EntityManager::EntityResolveComponentDependencies(const Entity::Pointer& entityPointer)
{
    AssertEntityPointerValid(entityPointer);
    auto& components = mEntityComponents[entityPointer.mIndex].mComponents;
    for (auto& component : components)
    {
        component->OnResolveDependencies();
    }
}

#if defined(_DEBUG)
bool EntityManager::IsComponentRegistered(const std::string& componentName) const
{
    return mRegisteredComponents.find(componentName) != mRegisteredComponents.end();
}

void EntityManager::AssertComponentRegistered(const std::string& componentName) const
{
    if (!IsComponentRegistered(componentName))
    {
        throw std::logic_error(std::string{ "EntityManager::AssertComponentRegistered: Component " } + componentName + std::string{ " not registered" });
    }
}
#endif

EntityManager::Iterator EntityManager::begin()
{
    return { *this, 0 };
}

EntityManager::Iterator EntityManager::end()
{
    return { *this, static_cast<Entity::PointerSize>(mEntityComponents.size()) };
}

EntityManager::ConstIterator EntityManager::begin() const
{
    return { *this, 0 };
}

EntityManager::ConstIterator EntityManager::end() const
{
    return { *this, static_cast<Entity::PointerSize>(mEntityComponents.size()) };
}

bool EntityManager::EntityComponentContainer::operator==(const EntityManager::EntityComponentContainer& other)
{
    return mCreated == other.mCreated && mComponents == other.mComponents;
}