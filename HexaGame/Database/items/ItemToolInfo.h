#pragma once

#include "ItemInfo.h"

class ItemToolInfo : public ItemInfo
{
public:
    ItemToolInfo(
        const Name& key,
        const Set<Name>& tags,
        const Shared<Texture>& icon,
        const Shared<StaticMesh>& mesh,
        const Shared<Texture>& mesh_texture,
        ItemType type,
        const String& name,
        uint stack_size,
        float mass,
        ItemTileTarget tile_target
        )
        : ItemInfo(key, tags, icon, mesh, mesh_texture, type, name, stack_size, mass, tile_target)
    {}

    void apply_to_tile(ItemContainer& item, const Shared<Character>& character, const TileIndex& world_index, const Shared<HexaWorld>& world) const override;
};
