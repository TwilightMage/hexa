#include "ItemToolInfo.h"

#include "CharacterInventory.h"
#include "HexaGame.h"
#include "Tiles.h"
#include "Entities/Character.h"
#include "Worlds/HexaWorld.h"

void ItemToolInfo::apply_to_tile(ItemContainer& item, const Shared<Character>& character, const TileIndex& index, const Shared<HexaWorld>& world) const
{
    if (Vector3::distance(character->get_position(), index.to_vector()) <= character->get_reach_distance())
    {
        auto tile_id = world->get_tile_id(index);
        if (tile_id != Tiles::air)
        {
            auto drops = tile_id->get_drops(index, world);
            world->set_tile(index, Tiles::air);
            for (auto& drop : drops)
            {
                if (!character->get_inventory()->add_item(drop))
                {
                    world->spawn_drop(index, drop);
                }
            }
        }
    }
}
