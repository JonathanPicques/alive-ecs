#include <ostream>
#include <istream>

#include "core/component.hpp"

DEFINE_ROOT_COMPONENT(Component);

Component::Component() : mEntity(nullptr, 0 , 0)
{

}

Component::~Component() // NOLINT
{

}

void Component::OnLoad()
{
    OnResolveDependencies();
}

void Component::OnResolveDependencies()
{

}

void Component::Serialize(std::ostream&) const
{

}

void Component::Deserialize(std::istream&)
{

}

#undef DEFINE_ROOT_COMPONENT