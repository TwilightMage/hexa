#include "ItemToolInfo.h"

#include "CharacterInventory.h"
#include "HexaGame.h"
#include "Tiles.h"
#include "Entities/Character.h"
#include "Worlds/HexaWorld.h"

void ItemToolInfo::apply_to_tile(ItemContainer& item, const Shared<Character>& character, const TileIndex& tile, const Shared<HexaWorld>& world) const
{
    if (Vector3::distance(character->get_position(), tile.to_vector()) <= character->get_reach_distance())
    {
        auto tile_id = world->get_tile_id(tile);
        if (tile_id != Tiles::air)
        {
            auto drop = HexaGame::item_database->get(tile_id->key);
            world->set_tile(tile, Tiles::air);
            if (drop)
            {
                if (!character->get_inventory()->add_item(ItemContainer(drop)))
                {
                    world->spawn_drop(tile, ItemContainer(drop));
                }
            }
        }
    }
}
