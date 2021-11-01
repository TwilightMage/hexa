#pragma once

#include "ItemContainer.h"
#include "MetaTags.h"
#include "TileIndex.h"
#include "TileType.h"
#include "Engine/DatabaseEntry.h"
#include "Engine/Game.h"
#include "Engine/Set.h"

class ComplexTileCustomData;
class ComplexTile;
class HexaWorld;
class Material3D;
class StaticMesh;
class Texture;

class TileInfo : public DatabaseEntry
{
public:
    TileInfo(
        const Name& key,
        TileType type,
        const Set<Name> tags,
        float hardness,
        bool block_nav
        );
    
    TileType type;
    const Set<Name> tags;
    float hardness;
    bool block_nav;
    Shared<Material> material;

    virtual void neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, ConstPtr<TileInfo> new_neighbor) const;
    virtual void on_tile_break(const TileIndex& index, const Shared<HexaWorld>& world) const;
    virtual List<ItemContainer> get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const;
    FORCEINLINE virtual TileSide get_collision_sides(const TileIndex& world_index, const HexaWorld* world) const { return TileSide::All; }
};

class AirTileInfo : public TileInfo
{
public:
    AirTileInfo(const Name& key)
    : TileInfo(key, TileType::Air, { MetaTags::AIR }, 0.0f, false)
    {}

    FORCEINLINE TileSide get_collision_sides(const TileIndex& world_index, const HexaWorld* world) const override { return TileSide::None; }
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
        : TileInfo(key, TileType::Solid, tags + Set{ MetaTags::SOLID }, hardness, true)
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
        bool block_nav,
        const Shared<StaticMesh>& mesh,
        const Shared<Texture>& texture,
        const Shared<Material>& material
        )
        : TileInfo(key, TileType::Complex, tags + Set{ MetaTags::COMPLEX }, hardness, block_nav)
        , mesh(mesh)
        , texture(texture)
        , material(material)
    {}

    void post_loading() override;
    
    virtual void setup_spawned_entity(const Shared<ComplexTile>& new_tile, const Shared<ComplexTileCustomData>& custom_data) const;
    virtual void on_tile_destroyed(const TileIndex& index, const Shared<ComplexTile>& destroyed_tile, const Shared<HexaWorld>& world) const;
    virtual bool can_place_at(const Shared<ComplexTile>& new_tile, const ItemContainer& item) const;
    virtual Shared<ComplexTileCustomData> create_custom_data() const;

    Shared<StaticMesh> mesh;
    Shared<Texture> texture;
    Shared<Material> material;
};