#pragma once

#include "ItemInfo.h"

class EXPORT ItemTileInfo : public ItemInfo
{
public:
    ItemTileInfo(
        const String& key,
        const Shared<Texture>& icon,
        const Shared<Mesh>& mesh,
        const Shared<Texture>& mesh_texture,
        ItemType type,
        const String& name,
        float mass
        )
        : ItemInfo(key, icon, mesh, mesh_texture, type, name, 99, mass, TARGET_OUTSIDE)
    {}
    
    void apply_to_tile(ItemContainer& item, const Shared<Character>& character, const TileIndex& world_index, const Shared<HexaWorld>& world) const override;
};
