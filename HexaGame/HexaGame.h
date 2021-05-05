#pragma once

#include "Engine/Game.h"

EXTERN class EXPORT HexaGame : public Game
{
public:
    HexaGame(int argc, char* argv[]);

protected:
    void init_game_info(GameInfo& outInfo) override;
    void start() override;
    
private:
    
};
