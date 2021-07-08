#pragma once

#include "ItemContainer.h"
#include "TileIndex.h"
#include "TileType.h"
#include "Engine/DatabaseEntry.h"

class ComplexTile;
class HexaWorld;
class Material3D;
class Mesh;
class Texture;

class TileInfo : public DatabaseEntry
{
public:
    TileInfo(
        const String& key,
        TileType type
        )
        : DatabaseEntry(key)
        , type(type)
    {}
    
    TileType type;

    virtual void neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, const Shared<const TileInfo>& new_neighbor) const;
    virtual List<ItemContainer> get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const;
};

class AirTileInfo : public TileInfo
{
public:
    AirTileInfo(const String& key)
    : TileInfo(key, TileType::Air)
    {}
};

class SolidTileInfo : public TileInfo
{
public:
    SolidTileInfo(
        const String& key,
        const Shared<Texture>& texture,
        bool randomize_ceil_uv_angle,
        bool randomize_floor_uv_angle
        )
        : TileInfo(key, TileType::Solid)
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
        const String& key,
        const Shared<Mesh>& mesh,
        const Shared<Texture>& texture,
        const Shared<Material3D>& material
        )
        : TileInfo(key, TileType::Complex)
        , mesh(mesh)
        , texture(texture)
        , material(material)
    {}

    virtual void setup_spawned_entity(const Shared<ComplexTile>& new_tile) const;

    Shared<Mesh> mesh;
    Shared<Texture> texture;
    Shared<Material3D> material;
};