#include "ItemTileInfo.h"

#include "HexaGame/HexaGame.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/Entities/Character.h"
#include "HexaGame/Worlds/HexaWorld.h"

void ItemTileInfo::apply_to_tile(ItemContainer& item, const Shared<Character>& character, const TileIndex& world_index, const Shared<HexaWorld>& world) const
{
    if (character->can_reach(world_index) && world->get_character_at(world_index) == nullptr && world->get_tile_id(world_index) == Tiles::air)
    {
        auto tile_info = HexaGame::tile_database->get(key);
        world->set_tile(world_index, tile_info);
        item.count--; 
    }  
}
