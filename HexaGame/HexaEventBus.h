#pragma once

#include "CharacterInfo.h"
#include "ItemInfo.h"
#include "TileInfo.h"
#include "Engine/Database.h"
#include "Engine/EventBus.h"

class Character;

class EXPORT HexaEventBus : public EventBus
{
public:
    Delegate<const Shared<Database<TileInfo>>&> on_load_tiles;
    Delegate<const Shared<Database<ItemInfo>>&> on_load_items;
    Delegate<const Shared<Database<CharacterInfo>>&> on_load_characters;
};
