#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/framework.h"

class Character;

class EXPORT CharacterController
{
public:
    virtual void posses_character(const Shared<Character>& character);
    virtual void un_posses_character();
    
    FORCEINLINE const Shared<Character>& get_character() const { return character_; }

    virtual Shared<const CharacterController> get_as_character_controller() const = 0;
    virtual Shared<CharacterController> get_as_character_controller() = 0;

private:
    Shared<Character> character_;
};
