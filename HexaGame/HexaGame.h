#pragma once

#include "Engine/Game.h"

EXTERN class EXPORT HexaGame : public Game
{
public:
    HexaGame(int argc, char* argv[]);

protected:
    virtual void init_game_info(GameInfo& outInfo) override;
    virtual void start() override;
    
private:
    
};
