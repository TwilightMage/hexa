#pragma once

#include "HexaGame.h"
#include "Engine/DatabaseUtils.h"
#include "Engine/Texture.h"

class Tiles
{
    BEGIN_RECORDS(TileInfo);
    RECORD(TileInfo, dirt, true, true, Texture::load_png("resources/hexagame/textures/tiles/dirt.png"), "");
    RECORD(TileInfo, grass, true, true, Texture::load_png("resources/hexagame/textures/tiles/grass.png"), "");
    RECORD(TileInfo, stone, true, true, Texture::load_png("resources/hexagame/textures/tiles/stone.png"), "");
    RECORD(TileInfo, sand, true, true, Texture::load_png("resources/hexagame/textures/tiles/sand.png"), "");
    RECORD(TileInfo, iron_ore, true, true, Texture::load_png("resources/hexagame/textures/tiles/iron_ore.png"), "");
    RECORD(TileInfo, stone_bricks, true, true, Texture::load_png("resources/hexagame/textures/tiles/stone_bricks.png"), "");
    END_RECORDS(TileInfo);
};
