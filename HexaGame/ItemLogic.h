#pragma once

#include "Engine/Compound.h"
#include "Engine/Pointers.h"
#include "Engine/Vector3.h"

class Character;

class ItemLogic
{
public:
    virtual void tick(const Shared<InventoryItem>& item, float delta_time) {}
    virtual void on_pick(const Shared<InventoryItem>& item, const Shared<Character>& character) {};
    virtual void on_drop(const Shared<InventoryItem>& item, const Shared<Character>& character, const Vector3& world_position) {};
};
