#include <cstring>

#include "core/component.hpp"

DEFINE_COMPONENT(Component);

Component::~Component() // NOLINT
{

}

void Component::Save(std::ostream& out) const
{

}