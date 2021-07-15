#pragma once

#include "ItemContainer.h"
#include "MetaTags.h"
#include "TileIndex.h"
#include "TileType.h"
#include "Engine/DatabaseEntry.h"
#include "Engine/Set.h"

class ComplexTileCustomData;
class ComplexTile;
class HexaWorld;
class Material3D;
class Mesh;
class Texture;

class TileInfo : public DatabaseEntry
{
public:
    TileInfo(
        const Name& key,
        TileType type,
        const Set<Name> tags,
        float hardness
        )
        : DatabaseEntry(key)
        , type(type)
        , tags(tags)
        , hardness(hardness)
    {}
    
    TileType type;
    const Set<Name> tags;
    float hardness;

    virtual void neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, const Shared<const TileInfo>& new_neighbor) const;
    virtual List<ItemContainer> get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const;
};

class AirTileInfo : public TileInfo
{
public:
    AirTileInfo(const Name& key)
    : TileInfo(key, TileType::Air, { MetaTags::AIR }, 0.0f)
    {}
};

class SolidTileInfo : public TileInfo
{
public:
    SolidTileInfo(
        const Name& key,
        const Set<Name>& tags,
        float hardness,
        const Shared<Texture>& texture,
        bool randomize_ceil_uv_angle,
        bool randomize_floor_uv_angle
        )
        : TileInfo(key, TileType::Solid, tags + Set{ MetaTags::SOLID }, hardness)
        , texture(texture)
        , randomize_ceil_uv_angle(randomize_ceil_uv_angle)
        , randomize_floor_uv_angle(randomize_floor_uv_angle)
    {}
    
    Shared<Texture> texture;

    bool randomize_ceil_uv_angle;
    bool randomize_floor_uv_angle;
};

class ComplexTileInfo : public TileInfo
{
public:
    ComplexTileInfo(
        const Name& key,
        const Set<Name>& tags,
        float hardness,
        const Shared<Mesh>& mesh,
        const Shared<Texture>& texture,
        const Shared<Material3D>& material
        )
        : TileInfo(key, TileType::Complex, tags + Set{ MetaTags::COMPLEX }, hardness)
        , mesh(mesh)
        , texture(texture)
        , material(material)
    {}

    virtual void setup_spawned_entity(const Shared<ComplexTile>& new_tile, const Shared<ComplexTileCustomData>& custom_data) const;
    virtual void on_tile_destroyed(const TileIndex& index, const Shared<ComplexTile>& destroyed_tile, const Shared<HexaWorld>& world) const;
    virtual bool can_place_at(const Shared<ComplexTile>& new_tile, const ItemContainer& item) const;
    virtual Shared<ComplexTileCustomData> create_custom_data() const;

    Shared<Mesh> mesh;
    Shared<Texture> texture;
    Shared<Material3D> material;
};