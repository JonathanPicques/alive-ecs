#pragma once

#include <ostream>

#define DEFINE_COMPONENT(NAME) constexpr const char* NAME::ComponentName; const char* NAME::GetComponentName() const { return NAME::ComponentName; }
#define DECLARE_COMPONENT(NAME) static constexpr const char* ComponentName{#NAME}; virtual const char *GetComponentName() const

class Entity;

class Component
{
public:
    DECLARE_COMPONENT(Component);

public:
    friend Entity;

public:
    virtual ~Component() = 0;

public:
    virtual void Serialize(std::ostream &os) const;
    virtual void Deserialize(std::istream &is);

protected:
    Entity* mEntity = nullptr;
};