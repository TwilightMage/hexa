#include "ItemToolInfo.h"

#include "HexaGame/CharacterInventory.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/Entities/Character.h"
#include "HexaGame/Worlds/HexaWorld.h"

void ItemToolInfo::apply_to_tile(ItemContainer& item, const Shared<Character>& character, const TileIndex& world_index, const Shared<HexaWorld>& world) const
{
    if (character->can_reach(world_index))
    {
        auto tile_id = world->get_tile_id(world_index);
        if (tile_id != Tiles::air)
        {
            auto drops = tile_id->get_drops(world_index, world);
            world->set_tile(world_index, Tiles::air);
            for (auto& drop : drops)
            {
                if (!character->get_inventory()->add_item(drop))
                {
                    world->spawn_drop(world_index, drop);
                }
            }
        }
    }
}
