#pragma once

#include <functional>

#include "Engine/DatabaseEntry.h"
#include "Engine/Pointers.h"

class Character;

class EXPORT CharacterInfo : public DatabaseEntry
{
public:
    CharacterInfo(
        const Name& key,
        std::function<Shared<Character>()> character_supplier
        )
        : DatabaseEntry(key)
        , character_supplier(character_supplier)
    {}
    
    std::function<Shared<Character>()> character_supplier;
};
