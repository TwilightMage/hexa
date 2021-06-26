#include "Characters.h"

#include "Entities/Characters/Slime.h"

DATABASE_INIT_FUNC_IMPL(Characters, CharacterInfo)
{
    INIT_CHARACTER_DATABASE_ENTRY(slime, CharacterInfo, Slime);
}