#pragma once

#include "ItemInfo.h"
#include "ItemTileInfo.h"
#include "ItemToolInfo.h"
#include "Paths.h"
#include "Engine/DatabaseHelpers.h"
#include "Engine/Texture.h"

#define INIT_ITEM_DATABASE_ENTRY(name, type, resources_path_root, mesh_name, ...) INIT_DATABASE_ENTRY(name, type, Texture::load_png(resources_path_root##_TEXTURES_ITEMS + #name + ".png"), Mesh::load_obj(resources_path_root##_MESHES_ITEMS + mesh_name + ".obj"), Texture::load_png(resources_path_root##_TEXTURES_ITEMS + #name + "_mesh.png"), __VA_ARGS__)

class Items
{
public:
    DECLARE_DATABASE_ENTRY(ItemToolInfo, iron_shovel);
    DECLARE_DATABASE_ENTRY(ItemTileInfo, stone_bricks);

    DATABASE_INIT_FUNC(ItemInfo)
    {
        INIT_ITEM_DATABASE_ENTRY(iron_shovel, ItemToolInfo, RESOURCES_HEXA, "shovel", ItemType::Shovel, "Iron Shovel", 1, 1.0f, TARGET_INSIDE);
        INIT_ITEM_DATABASE_ENTRY(stone_bricks, ItemTileInfo, RESOURCES_HEXA, "bricks", ItemType::Tile, "Stone Bricks", 99, 1.0f, TARGET_OUTSIDE);
    }
};
