#include "CharacterController.h"

#include "Character.h"

void CharacterController::posses_character(const Shared<Character>& character)
{
    un_posses_character();

    if (auto controller = character->controller_.lock())
    {
        controller->un_posses_character();
    }

    character_ = character;
    character_->controller_ = get_as_character_controller();
    character_->on_character_possesed();
}

void CharacterController::un_posses_character()
{
    if (character_)
    {
        character_->on_character_un_possesed();
        character_->controller_ = null_weak(CharacterController);
        character_ = nullptr;
    }
}

Shared<Character> CharacterController::get_character() const
{
    return character_;
}
