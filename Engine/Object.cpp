#include "Object.h"

Object::Object(const String& name)
    : name(name)
{
}

const String& Object::get_name() const
{
    return name;
}
