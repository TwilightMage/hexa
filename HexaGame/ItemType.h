#pragma once

enum class ItemType
{
    None = 0,
    Sword = 1,
    Shovel = 2,
    Axe = 4,
    Pickaxe = 8,
    Tile = 16,
    Resource = 32
};

IMPLEMENT_ENUM_FLAGS(ItemType);