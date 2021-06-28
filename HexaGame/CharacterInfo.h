#pragma once

#include "Engine/DatabaseEntry.h"

class Character;

class EXPORT CharacterInfo : public DatabaseEntry
{
public:
    CharacterInfo(
        const String& key,
        std::function<Shared<Character>()> character_supplier
        )
        : DatabaseEntry(key)
        , character_supplier(character_supplier)
    {}
    
    std::function<Shared<Character>()> character_supplier;
};
