#pragma once

#include "Engine/DatabaseEntry.h"
#include "Engine/Name.h"
#include "Engine/Set.h"
#include "HexaGame/ItemContainer.h"
#include "HexaGame/ItemType.h"
#include "HexaGame/TileIndex.h"

class Character;
class HexaWorld;
class Mesh;
class Texture;

enum ItemTileTarget
{
    TARGET_INSIDE,
    TARGET_OUTSIDE,
    TARGET_DEFAULT = TARGET_OUTSIDE
};

class EXPORT ItemInfo : public DatabaseEntry
{
public:
    ItemInfo(
        const Name& key,
        const Set<Name>& tags,
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
        , tags(tags)
        , icon(icon)
        , mesh(mesh)
        , mesh_texture(mesh_texture)
        , type(type)
        , name(name)
        , stack_size(stack_size)
        , mass(mass)
        , tile_target(tile_target)
    {}

    Set<Name> tags;
    
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
