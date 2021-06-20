#pragma once

#include "HexaGame.h"
#include "Paths.h"
#include "Engine/Texture.h"

class EXPORT Tiles
{
public:
    inline static Shared<const TileInfo> air = nullptr;
    inline static Shared<const TileInfo> dirt = nullptr;
    inline static Shared<const TileInfo> grass = nullptr;
    inline static Shared<const TileInfo> stone = nullptr;
    inline static Shared<const TileInfo> sand = nullptr;
    inline static Shared<const TileInfo> iron_ore = nullptr;
    inline static Shared<const TileInfo> stone_bricks = nullptr;

    static void init(const Shared<Database<TileInfo>>& database)
    {
        air = database->add({ "air", TileType::Air, false, false, nullptr, "" });
        dirt = database->add({ "dirt", TileType::Solid, true, true, Texture::load_png(RESOURCES_HEXA_TEXTURES_TILES + "dirt.png"), "" });
        grass = database->add({ "grass", TileType::Solid, true, true, Texture::load_png(RESOURCES_HEXA_TEXTURES_TILES + "grass.png"), "" });
        stone = database->add({ "stone", TileType::Solid, true, true, Texture::load_png(RESOURCES_HEXA_TEXTURES_TILES + "stone.png"), "" });
        sand = database->add({ "sand", TileType::Solid, true, true, Texture::load_png(RESOURCES_HEXA_TEXTURES_TILES + "sand.png"), "" });
        iron_ore = database->add({ "iron_ore", TileType::Solid, true, true, Texture::load_png(RESOURCES_HEXA_TEXTURES_TILES + "iron_ore.png"), "" });
        stone_bricks = database->add({ "stone_bricks", TileType::Solid, true, true, Texture::load_png(RESOURCES_HEXA_TEXTURES_TILES + "stone_bricks.png"), "" });
    }
};
