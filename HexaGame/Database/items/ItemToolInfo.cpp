#include "ItemToolInfo.h"

#include "HexaGame/CharacterInventory.h"
#include "HexaGame/MetaTags.h"
#include "HexaGame/ComplexTileCustomData/TreeStemCustomData.h"
#include "HexaGame/Database/Tiles.h"
#include "HexaGame/Entities/Character.h"
#include "HexaGame/Worlds/HexaWorld.h"

void ItemToolInfo::apply_to_tile(ItemContainer& item, const Shared<Character>& character, const TileIndex& world_index, const Shared<HexaWorld>& world) const
{
    if (character->can_reach(world_index))
    {
        auto tile_id = world->get_tile_id(world_index);
        if (tile_id != Tiles::air)
        {
            float tile_damage = 0.0f;
            switch (item.item->type)
            {
                case ItemType::Shovel:
                    if (tile_id->tags.contains(MetaTags::DIRT))
                    {
                        tile_damage = 0.5f;
                    }
                    else if (tile_id->tags.contains(MetaTags::GRASS))
                    {
                        tile_damage = 1.0f;
                    }
                    else if (tile_id->tags.contains(MetaTags::STONE))
                    {
                        tile_damage = 0.1f;
                    }

                    if (auto stem_data = cast<TreeStemCustomData>(world->get_custom_data(world_index.offset(0, 0, 1))))
                    {
                        if (stem_data->type == TreeStemCustomData::Type::Roots)
                        {
                            tile_damage /= 10.0f;
                        }
                    }
                    break;
                case ItemType::Axe:
                    if (tile_id->tags.contains(MetaTags::WOOD))
                    {
                        if (tile_id->tags.contains(MetaTags::STEM))
                        {
                            if (auto stem_data = cast<TreeStemCustomData>(world->get_custom_data(world_index)))
                            {
                                if (stem_data->type == TreeStemCustomData::Type::Roots)
                                {
                                    tile_damage = 0.1f;
                                }
                                else
                                {
                                    tile_damage = 0.2f;
                                }
                            }
                        }
                        else
                        {
                            tile_damage = 0.25f;
                        }

                        auto chop_sound_handle = world->play_sound_3d(HexaGame::wood_chop_sound, world_index.to_vector(), HexaGame::get_effects_channel());
                    }
                    break;
                case ItemType::Pickaxe:
                    if (tile_id->tags.contains(MetaTags::STONE))
                    {
                        tile_damage = 0.5f;
                    }
                    break;
            }

            if (tile_damage > 0 && world->damage_tile(world_index, tile_damage))
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
}
