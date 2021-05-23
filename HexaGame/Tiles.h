#pragma once

#include "HexaGame.h"
#include "Engine/DatabaseUtils.h"
#include "Engine/Texture.h"

class Tiles
{
    USE_DATABASE(TileInfo, HexaGame::tile_database);
    
    BEGIN_RECORDS();
    RECORD(TileInfo, dirt, Texture::load_png("resources/hexagame/textures/tiles/dirt.png"));
    RECORD(TileInfo, grass, Texture::load_png("resources/hexagame/textures/tiles/grass.png"));
    RECORD(TileInfo, stone, Texture::load_png("resources/hexagame/textures/tiles/stone.png"));
    RECORD(TileInfo, sand, Texture::load_png("resources/hexagame/textures/tiles/sand.png"));
    RECORD(TileInfo, iron_ore, Texture::load_png("resources/hexagame/textures/tiles/iron_ore.png"));
    RECORD(TileInfo, stone_bricks, Texture::load_png("resources/hexagame/textures/tiles/stone_bricks.png"));
    END_RECORDS();
};
