#include "ItemTileInfo.h"

#include "HexaGame.h"
#include "Tiles.h"
#include "Entities/Character.h"
#include "Worlds/HexaWorld.h"

void ItemTileInfo::apply_to_tile(ItemContainer& item, const Shared<Character>& character, const TileIndex& tile, const Shared<HexaWorld>& world) const
{
    if (Vector3::distance(character->get_position(), tile.to_vector()) <= character->get_reach_distance())
    {
        if (world->get_tile_id(tile) == Tiles::air)
        {
            auto tile_info = HexaGame::tile_database->get(key);
            world->set_tile(tile, tile_info);
            item.count--;
        }  
    }  
}
