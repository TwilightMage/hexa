#pragma once

#include "Engine/DatabaseHelpers.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/Paths.h"
#include "HexaGame/TileInfo.h"
#include "HexaGame/Database/tiles/GrassInfo.h"
#include "HexaGame/Database/tiles/TallGrassInfo.h"
#include "HexaGame/Database/tiles/TreeStemInfo.h"

#define LOAD_MESH(domain, subdomain, name) (StaticMesh::load_file_obj(domain##_##subdomain + name + ".obj"))
#define LOAD_TEX(module, name) (module->get_texture(String(name) + ".png"))

#define INIT_SOLID_TILE_DATABASE_ENTRY(name, type, tags, hardness, resources_path_root, module, ...)  INIT_DATABASE_ENTRY(name, type, tags, hardness, LOAD_TEX(module, #name), __VA_ARGS__);
#define INIT_COMPLEX_TILE_DATABASE_ENTRY(name, type, tags, hardness, resources_path_root, module, mesh_name, ...) INIT_DATABASE_ENTRY(name, type, tags, hardness, LOAD_MESH(resources_path_root, MESHES_TILES, mesh_name), LOAD_TEX(module, #name), __VA_ARGS__);
#define INIT_TREE_STEM_DATABASE_ENTRY(name, type, tags, hardness, resources_path_root, module, plant_name, drop_name, ...) INIT_COMPLEX_TILE_DATABASE_ENTRY(name, type, tags, hardness, resources_path_root, module, plant_name + "_stem", nullptr, LOAD_MESH(resources_path_root, MESHES_TILES, plant_name + "_root"), LOAD_TEX(module, String(#name) + "_root"), LOAD_MESH(resources_path_root, MESHES_TILES, plant_name + "_krone"), LOAD_TEX(module, String(#name) + "_krone"), LOAD_MESH(resources_path_root, MESHES_TILES, plant_name + "_branch"), LOAD_TEX(module, String(#name) + "_branch"), LOAD_MESH(resources_path_root, MESHES_TILES, plant_name + "_branch_krone"), LOAD_TEX(module, String(#name) + "_branch_krone"), drop_name)

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

    //DATABASE_INIT_FUNC(TileInfo)
    //{        
    //    INIT_DATABASE_ENTRY(air, AirTileInfo);
    //    
    //    INIT_SOLID_TILE_DATABASE_ENTRY(dirt, SolidTileInfo, { MetaTags::DIRT }, 1.0f, RESOURCES, Game::get_instance(), true, true);
    //    INIT_SOLID_TILE_DATABASE_ENTRY(grass, GrassInfo, { MetaTags::DIRT }, 1.0f, RESOURCES, Game::get_instance(), true, true);
    //    INIT_SOLID_TILE_DATABASE_ENTRY(stone, SolidTileInfo, { MetaTags::STONE }, 2.0f, RESOURCES, Game::get_instance(), true, true);
    //    INIT_SOLID_TILE_DATABASE_ENTRY(sand, SolidTileInfo, { MetaTags::DIRT }, 0.5f, RESOURCES, Game::get_instance(), true, true);
    //    INIT_SOLID_TILE_DATABASE_ENTRY(iron_ore, SolidTileInfo, { MetaTags::STONE }, 2.5f, RESOURCES, Game::get_instance(), true, true);
    //    INIT_SOLID_TILE_DATABASE_ENTRY(stone_bricks, SolidTileInfo, { MetaTags::STONE }, 2.0f, RESOURCES, Game::get_instance(), true, true);
    //
    //    INIT_COMPLEX_TILE_DATABASE_ENTRY(tall_grass, TallGrassInfo, {}, 0.1f, RESOURCES, Game::get_instance(), "tall_grass", nullptr);
    //    INIT_COMPLEX_TILE_DATABASE_ENTRY(blue_roses, TallGrassInfo, { MetaTags::FLOWER }, 0.1f, RESOURCES, Game::get_instance(), "tall_grass", nullptr);
    //    INIT_COMPLEX_TILE_DATABASE_ENTRY(red_roses, TallGrassInfo, { MetaTags::FLOWER }, 0.1f, RESOURCES, Game::get_instance(), "tall_grass", nullptr);
    //    INIT_COMPLEX_TILE_DATABASE_ENTRY(chamomile, TallGrassInfo, { MetaTags::FLOWER }, 0.1f, RESOURCES, Game::get_instance(), "tall_grass", nullptr);
    //
    //    INIT_COMPLEX_TILE_DATABASE_ENTRY(ash_stem, TreeStemInfo, {}, 1.0f, RESOURCES, Game::get_instance(), "tree" + "_stem", nullptr, LOAD_MESH(RESOURCES, MESHES_TILES, "tree" + "_root"), LOAD_TEX(Game::get_instance(), String("ash_stem") + "_root"), LOAD_MESH(RESOURCES, MESHES_TILES, "tree" + "_krone"), LOAD_TEX(Game::get_instance(), String("ash_stem") + "_krone"), LOAD_MESH(RESOURCES, MESHES_TILES, "tree" + "_branch"), LOAD_TEX(Game::get_instance(), String("ash_stem") + "_branch"), LOAD_MESH(RESOURCES, MESHES_TILES, "tree" + "_branch_krone"), LOAD_TEX(Game::get_instance(), String("ash_stem") + "_branch_krone"), "ash_log");
    //}
};
