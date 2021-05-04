#include "Object.h"

Object::Object(const String& name)
    : name_(name)
{
}

const String& Object::get_name() const
{
    return name_;
}
