#pragma once

#include "String.h"

class EXPORT DatabaseEntry
{
public:
    DatabaseEntry(const String& key)
        : key(key)
    {}
    
    String key;
};
