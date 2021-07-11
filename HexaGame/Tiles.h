#pragma once

#include "HexaGame.h"
#include "Paths.h"
#include "TileInfo.h"
#include "Database/tiles/TreeStemInfo.h"
#include "Engine/DatabaseHelpers.h"
#include "Engine/Mesh.h"
#include "Engine/Texture.h"
#include "HexaGame/Database/tiles/GrassInfo.h"
#include "HexaGame/Database/tiles/TallGrassInfo.h"

#define INIT_SOLID_TILE_DATABASE_ENTRY(name, type, tags, hardness, resources_path_root, ...)  INIT_DATABASE_ENTRY(name, type, tags, hardness, Texture::load_png(resources_path_root##_TEXTURES_TILES + #name + ".png"), __VA_ARGS__);
#define INIT_COMPLEX_TILE_DATABASE_ENTRY(name, type, tags, hardness, resources_path_root, mesh_name, ...) INIT_DATABASE_ENTRY(name, type, tags, hardness, Mesh::load_obj(resources_path_root##_MESHES_TILES + mesh_name + ".obj"), Texture::load_png(resources_path_root##_TEXTURES_COMPLEXTILES + #name + ".png"), __VA_ARGS__);

class EXPORT Tiles
{
public:
    DECLARE_DATABASE_ENTRY(AirTileInfo, air);

    DECLARE_DATABASE_ENTRY(SolidTileInfo, dirt);
    DECLARE_DATABASE_ENTRY(GrassInfo, grass);
    DECLARE_DATABASE_ENTRY(SolidTileInfo, stone);
    DECLARE_DATABASE_ENTRY(SolidTileInfo, sand);
    DECLARE_DATABASE_ENTRY(SolidTileInfo, iron_ore);
    DECLARE_DATABASE_ENTRY(SolidTileInfo, stone_bricks);

    DECLARE_DATABASE_ENTRY(TallGrassInfo, tall_grass);
    DECLARE_DATABASE_ENTRY(TallGrassInfo, blue_roses);
    DECLARE_DATABASE_ENTRY(TallGrassInfo, red_roses);
    DECLARE_DATABASE_ENTRY(TallGrassInfo, chamomile);

    DECLARE_DATABASE_ENTRY(TreeStemInfo, ash_stem);

    DATABASE_INIT_FUNC(TileInfo)
    {
        INIT_DATABASE_ENTRY(air, AirTileInfo);
        
        INIT_SOLID_TILE_DATABASE_ENTRY(dirt, SolidTileInfo, { MetaTags::DIRT }, 1.0f, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(grass, GrassInfo, { MetaTags::DIRT }, 1.0f, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(stone, SolidTileInfo, { MetaTags::STONE }, 2.0f, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(sand, SolidTileInfo, { MetaTags::DIRT }, 0.5f, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(iron_ore, SolidTileInfo, { MetaTags::STONE }, 2.5f, RESOURCES_HEXA, true, true);
        INIT_SOLID_TILE_DATABASE_ENTRY(stone_bricks, SolidTileInfo, { MetaTags::STONE }, 2.0f, RESOURCES_HEXA, true, true);

        INIT_COMPLEX_TILE_DATABASE_ENTRY(tall_grass, TallGrassInfo, {}, 0.1f, RESOURCES_HEXA, "tall_grass", HexaGame::foliage_material);
        INIT_COMPLEX_TILE_DATABASE_ENTRY(blue_roses, TallGrassInfo, { MetaTags::FLOWER }, 0.1f, RESOURCES_HEXA, "tall_grass", HexaGame::foliage_material);
        INIT_COMPLEX_TILE_DATABASE_ENTRY(red_roses, TallGrassInfo, { MetaTags::FLOWER }, 0.1f, RESOURCES_HEXA, "tall_grass", HexaGame::foliage_material);
        INIT_COMPLEX_TILE_DATABASE_ENTRY(chamomile, TallGrassInfo, { MetaTags::FLOWER }, 0.1f, RESOURCES_HEXA, "tall_grass", HexaGame::foliage_material);

        INIT_COMPLEX_TILE_DATABASE_ENTRY(ash_stem, TreeStemInfo, {}, 0.1f, RESOURCES_HEXA, "tree_stem", Game::get_basic_material_3d(),
        { Mesh::load_obj(RESOURCES_HEXA_MESHES_TILES + "tree_root_0.obj"), Mesh::load_obj(RESOURCES_HEXA_MESHES_TILES + "tree_root_1.obj"), Mesh::load_obj(RESOURCES_HEXA_MESHES_TILES + "tree_root_2.obj") },
        { Texture::load_png(RESOURCES_HEXA_TEXTURES_COMPLEXTILES + "ash_root_0.png"), Texture::load_png(RESOURCES_HEXA_TEXTURES_COMPLEXTILES + "ash_root_1.png"), Texture::load_png(RESOURCES_HEXA_TEXTURES_COMPLEXTILES + "ash_root_2.png") },
        Mesh::load_obj(RESOURCES_HEXA_MESHES_TILES + "tree_krone.obj"),
        Texture::load_png(RESOURCES_HEXA_TEXTURES_COMPLEXTILES + "ash_krone.png")
        );
    }
};
