#pragma once

#include "ItemContainer.h"
#include "ItemType.h"
#include "TileIndex.h"
#include "Engine/DatabaseEntry.h"

class Character;
class HexaWorld;
class Mesh;
class Texture;

enum ItemTileTarget
{
    TARGET_INSIDE,
    TARGET_OUTSIDE
};

class EXPORT ItemInfo : public DatabaseEntry
{
public:
    ItemInfo(
        const String& key,
        const Shared<Texture>& icon,
        const Shared<Mesh>& mesh,
        const Shared<Texture>& mesh_texture,
        ItemType type,
        const String& name,
        uint stack_size,
        float mass,
        ItemTileTarget tile_target
        )
        : DatabaseEntry(key)
        , icon(icon)
        , mesh(mesh)
        , mesh_texture(mesh_texture)
        , type(type)
        , name(name)
        , stack_size(stack_size)
        , mass(mass)
        , tile_target(tile_target)
    {}

    Shared<Texture> icon;

    Shared<Mesh> mesh;
    Shared<Texture> mesh_texture;

    ItemType type;
    String name;
    uint stack_size;
    float mass;
    ItemTileTarget tile_target;

    virtual void apply_to_tile(ItemContainer& item, const Shared<Character>& character, const TileIndex& tile, const Shared<HexaWorld>& world) const {}
};
