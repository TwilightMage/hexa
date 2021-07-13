#pragma once

#include "Paths.h"
#include "Engine/DatabaseHelpers.h"
#include "Engine/Mesh.h"
#include "Engine/Texture.h"
#include "HexaGame/Database/items/ItemInfo.h"
#include "HexaGame/Database/items/ItemTileInfo.h"
#include "HexaGame/Database/items/ItemToolInfo.h"

#define INIT_ITEM_DATABASE_ENTRY(name, type, resources_path_root, mesh_name, ...) INIT_DATABASE_ENTRY(name, type, Texture::load_png(resources_path_root##_TEXTURES_ITEMS + #name + ".png"), Mesh::load_obj(resources_path_root##_MESHES_ITEMS + mesh_name + ".obj"), Texture::load_png(resources_path_root##_TEXTURES_ITEMS + #name + "_mesh.png"), __VA_ARGS__)

class Items
{
public:
    DECLARE_DATABASE_ENTRY(ItemToolInfo, iron_shovel);
    DECLARE_DATABASE_ENTRY(ItemTileInfo, stone_bricks);
    DECLARE_DATABASE_ENTRY(ItemTileInfo, dirt);
    DECLARE_DATABASE_ENTRY(ItemInfo,     ash_log);

    DATABASE_INIT_FUNC(ItemInfo)
    {
        INIT_ITEM_DATABASE_ENTRY(iron_shovel,  ItemToolInfo, RESOURCES_HEXA, "shovel", ItemType::Shovel,                        "Iron Shovel",  1, 1.0f, TARGET_INSIDE);
        INIT_ITEM_DATABASE_ENTRY(stone_bricks, ItemTileInfo, RESOURCES_HEXA, "bricks", ItemType::Tile | ItemType::Resource,     "Stone Bricks", 1.0f);
        INIT_ITEM_DATABASE_ENTRY(dirt,         ItemTileInfo, RESOURCES_HEXA, "pile",   ItemType::Tile | ItemType::Resource,     "Dirt Pile",    1.0f);
        INIT_ITEM_DATABASE_ENTRY(ash_log,      ItemInfo,     RESOURCES_HEXA, "log",    ItemType::Resource,                      "Ash Log",      99, 1.0f, TARGET_DEFAULT);
    }
};
