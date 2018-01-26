#include <cstring>

#include "core/component.hpp"

DEFINE_COMPONENT(Component);

Component::~Component() // NOLINT
{

}

void Component::Serialize(std::ostream& os) const
{

}

void Component::Deserialize(std::istream& is)
{

}