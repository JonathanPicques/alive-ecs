#include <cstring>

#include "core/component.hpp"

DEFINE_COMPONENT(Component);

Component::~Component() // NOLINT
{

}

void Component::OnComponentLoaded()
{

}

void Component::OnAllComponentLoaded()
{

}

void Component::Serialize(std::ostream&) const
{

}

void Component::Deserialize(std::istream&)
{
    OnComponentLoaded();
}