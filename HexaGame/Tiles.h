#pragma once

#include "Paths.h"
#include "TileInfo.h"
#include "Engine/DatabaseHelpers.h"
#include "Engine/Texture.h"

#define INIT_TILE_DATABASE_ENTRY(name, type, resources_path_root, ...)  INIT_DATABASE_ENTRY(name, type, Texture::load_png(resources_path_root##_TEXTURES_TILES + #name + ".png"), __VA_ARGS__);

class EXPORT Tiles
{
public:
    DECLARE_DATABASE_ENTRY(TileInfo, air);
    DECLARE_DATABASE_ENTRY(TileInfo, dirt);
    DECLARE_DATABASE_ENTRY(TileInfo, grass);
    DECLARE_DATABASE_ENTRY(TileInfo, stone);
    DECLARE_DATABASE_ENTRY(TileInfo, sand);
    DECLARE_DATABASE_ENTRY(TileInfo, iron_ore);
    DECLARE_DATABASE_ENTRY(TileInfo, stone_bricks);

    DATABASE_INIT_FUNC(TileInfo)
    {
        INIT_DATABASE_ENTRY(air, TileInfo, nullptr, TileType::Air, false, false);
        INIT_TILE_DATABASE_ENTRY(dirt, TileInfo, RESOURCES_HEXA, TileType::Solid, true, true);
        INIT_TILE_DATABASE_ENTRY(grass, TileInfo, RESOURCES_HEXA, TileType::Solid, true, true);
        INIT_TILE_DATABASE_ENTRY(stone, TileInfo, RESOURCES_HEXA, TileType::Solid, true, true);
        INIT_TILE_DATABASE_ENTRY(sand, TileInfo, RESOURCES_HEXA, TileType::Solid, true, true);
        INIT_TILE_DATABASE_ENTRY(iron_ore, TileInfo, RESOURCES_HEXA, TileType::Solid, true, true);
        INIT_TILE_DATABASE_ENTRY(stone_bricks, TileInfo, RESOURCES_HEXA, TileType::Solid, true, true);
    }
};
