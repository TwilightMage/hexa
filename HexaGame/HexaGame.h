#pragma once

#include "TileInfo.h"
#include "Engine/Database.h"
#include "Engine/Game.h"

EXTERN class EXPORT HexaGame : public Game
{
public:
    HexaGame(int argc, char* argv[]);

    static Shared<Database<TileInfo>> tile_database;
    
protected:
    void init_game_info(GameInfo& outInfo) override;
    void start() override;
    void loading_stage() override;
    
};
