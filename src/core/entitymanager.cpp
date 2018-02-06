#include <cstring>
#include <sstream>
#include <ostream>
#include <istream>
#include <cstring>
#include <algorithm>

#include "core/entitymanager.hpp"

Entity EntityManager::CreateEntity()
{
    Entity::PointerSize index;
    Entity::PointerSize version;
    if (mFreeIndexes.empty())
    {
        index = mNextIndex++;
        mVersions.resize(index + 1);
        mEntityComponents.resize(index + 1);
        version = mVersions[index] = 1;
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
    mEntityComponents[entityPointer.mIndex].clear();
    mFreeIndexes.push_back(entityPointer.mIndex);
}

void EntityManager::Serialize(std::ostream& os) const
{
    for (auto entity : *this)
    {
        os << '{';
        os.write(reinterpret_cast<char*>(&entity.mPointer.mIndex), sizeof(entity.mPointer.mIndex));
        os.write(reinterpret_cast<char*>(&entity.mPointer.mVersion), sizeof(entity.mPointer.mVersion));
        for (const auto& component : mEntityComponents[entity.mPointer.mIndex])
        {
            os.write(component->GetComponentName().c_str(), 1 + component->GetComponentName().size());
            component->Serialize(os);
        }
        os << '}';
    }
}

void EntityManager::Deserialize(std::istream& is)
{

    mNextIndex = 0;
    mVersions.clear();
    mFreeIndexes.clear();
    mEntityComponents.clear(); // TODO: clear entity manager

    enum class ParsingState
    {
        eEntity,
        eComponentName,
    };
    auto state = ParsingState::eEntity;
    std::unique_ptr<Entity> entity;
    std::string componentName;
    while (!is.eof())
    {
        char token;
        is >> token;
        if (state == ParsingState::eEntity)
        {
            if (token == '0')
            {
                break;
            }
            else if (token == '{')
            {
                entity = std::make_unique<Entity>(this, Entity::Pointer(0, 0));
                is.read(reinterpret_cast<char*>(&entity->mPointer.mIndex), sizeof(entity->mPointer.mIndex));
                is.read(reinterpret_cast<char*>(&entity->mPointer.mVersion), sizeof(entity->mPointer.mVersion));
                for (auto i = mNextIndex; i < entity->mPointer.mIndex; i++)
                {
                    mFreeIndexes.emplace_back(i);
                }
                mNextIndex = static_cast<Entity::PointerSize>(entity->mPointer.mIndex + 1);
                mVersions.resize(mNextIndex);
                mEntityComponents.resize(mNextIndex);
                mVersions[entity->mPointer.mIndex] = entity->mPointer.mVersion;
                state = ParsingState::eComponentName;
            }
        }
        else if (state == ParsingState::eComponentName)
        {
            if (token == '}')
            {
                entity->ResolveComponentDependencies();
                state = ParsingState::eEntity;
            }
            else if (token == '\0')
            {
                auto componentCreator = mRegisteredComponents.find(componentName);
                if (componentCreator == mRegisteredComponents.end())
                {
                    throw std::logic_error(componentName + std::string { " is not registered" });
                }
                auto component = componentCreator->second();
                auto componentPtr = component.get();
                mEntityComponents[entity->mPointer.mIndex].emplace_back(std::move(component));
                componentPtr->Deserialize(is);
                EntityConstructComponent(componentPtr, entity->mPointer);
                componentName.clear();
                state = ParsingState::eComponentName;
            }
            else
            {
                componentName += token;
            }
        }
    }
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
    auto& components = mEntityComponents[entityPointer.mIndex];
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