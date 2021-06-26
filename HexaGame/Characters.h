#pragma once

#include "CharacterInfo.h"
#include "Engine/DatabaseHelpers.h"

#define INIT_CHARACTER_DATABASE_ENTRY(name, type, sub_type, ...) INIT_DATABASE_ENTRY(name, type, []()->Shared<Character>{ return MakeShared<sub_type>(); }, __VA_ARGS__)

class Slime;

class Characters
{
public:
    DECLARE_DATABASE_ENTRY(CharacterInfo, slime);

    DATABASE_INIT_FUNC(CharacterInfo);
};
