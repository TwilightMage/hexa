#pragma once

#include "CharacterInfo.h"
#include "ItemInfo.h"
#include "TileInfo.h"
#include "Engine/Table.h"
#include "Engine/EventBus.h"

class Character;

class EXPORT HexaEventBus : public EventBus
{
public:
    Delegate<const Shared<Table<TileInfo>>&> on_load_tiles;
    Delegate<const Shared<Table<ItemInfo>>&> on_load_items;
    Delegate<const Shared<Table<CharacterInfo>>&> on_load_characters;
};
