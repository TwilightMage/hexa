#pragma once

#include "Name.h"
#include "String.h"

class EXPORT DatabaseEntry
{
public:
    DatabaseEntry(const Name& key)
        : key(key)
    {}
    
    Name key;
};
