#pragma once

#include "String.h"

class EXPORT Object
{
public:
    Object(const String& name);

    const String& get_name() const;

    String name;
};
