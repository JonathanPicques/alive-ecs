#include <cstring>

#include "core/component.hpp"

DEFINE_COMPONENT(Component);

Component::~Component() // NOLINT
{

}

void Component::Save(std::ostream& os) const
{

}

void Component::Load(std::istream& is)
{

}