#pragma once

enum class ItemType
{
    None = 0,
    Sword = 1,
    Shovel = 2,
    Axe = 4,
    Tile = 8,
    Resource = 16
};

IMPLEMENT_ENUM_FLAGS(ItemType);