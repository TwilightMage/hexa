#pragma once

#include "HexaGame.h"
#include "Paths.h"
#include "TileInfo.h"
#include "Engine/DatabaseHelpers.h"
#include "Engine/Mesh.h"
#include "Engine/Texture.h"
#include "tiles/GrassTile.h"
#include "tiles/TallGrassTile.h"

#define INIT_SOLID_TILE_DATABASE_ENTRY(name, type, resources_path_root, ...)  INIT_DATABASE_ENTRY(name, type, Texture::load_png(resources_path_root##_TEXTURES_TILES + #name + ".png"), __VA_ARGS__);
#define INIT_COMPLEX_TILE_DATABASE_ENTRY(name, type, resources_path_root, mesh_name, ...) INIT_DATABASE_ENTRY(name, type, Mesh::load_obj(resources_path_root##_MESHES_TILES + mesh_name + ".obj"), Texture::load_png(resources_path_root##_TEXTURES_TILES_COMPLEX + #name + ".png"), __VA_ARGS__);

class EXPORT Tiles
{
public:
    DECLARE_DATABASE_ENTRY(AirTileInfo, air);

    DECLARE_DATABASE_ENTRY(SolidTileInfo, dirt);
    DECLARE_DATABASE_ENTRY(GrassTile, grass);
    DECLARE_DATABASE_ENTRY(SolidTileInfo, stone);
    DECLARE_DATABASE_ENTRY(SolidTileInfo, sand);
    DECLARE_DATABASE_ENTRY(SolidTileInfo, iron_ore);
    DECLARE_DATABASE_ENTRY(SolidTileInfo, stone_bricks);

    DECLARE_DATABASE_ENTRY(TallGrassTile, tall_grass);
    DECLARE_DATABASE_ENTRY(TallGrassTile, blue_roses);
    DECLARE_DATABASE_ENTRY(TallGrassTile, red_roses);
    DECLARE_DATABASE_ENTRY(TallGrassTile, chamomile);

    DATABASE_INIT_FUNC(TileInfo)
    {
        INIT_DATABASE_ENTRY(air, AirTileInfo);
        
        INIT_SOLID_TILE_DATABASE_ENTRY(dirt, SolidTileInfo, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(grass, GrassTile, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(stone, SolidTileInfo, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(sand, SolidTileInfo, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(iron_ore, SolidTileInfo, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(stone_bricks, SolidTileInfo, RESOURCES_HEXA, true, true);

        INIT_COMPLEX_TILE_DATABASE_ENTRY(tall_grass, TallGrassTile, RESOURCES_HEXA, "tall_grass", HexaGame::foliage_material);
        INIT_COMPLEX_TILE_DATABASE_ENTRY(blue_roses, TallGrassTile, RESOURCES_HEXA, "tall_grass", HexaGame::foliage_material);
        INIT_COMPLEX_TILE_DATABASE_ENTRY(red_roses, TallGrassTile, RESOURCES_HEXA, "tall_grass", HexaGame::foliage_material);
        INIT_COMPLEX_TILE_DATABASE_ENTRY(chamomile, TallGrassTile, RESOURCES_HEXA, "tall_grass", HexaGame::foliage_material);
    }
};
