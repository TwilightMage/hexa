#pragma once

#include "Engine/DatabaseEntry.h"

class Character;

class EXPORT CharacterInfo : public DatabaseEntry
{
public:
    std::function<Shared<Character>()> character_supplier;
};
