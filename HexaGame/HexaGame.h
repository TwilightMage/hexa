#pragma once

#include "Engine/Game.h"

EXTERN class EXPORT HexaGame : public Game
{
public:
    HexaGame(int argc, char* argv[]);

protected:
    void InitGameInfo(GameInfo& outInfo) override;

private:
    
};
