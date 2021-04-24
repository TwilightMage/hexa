#include "HexaGame.h"

HexaGame::HexaGame(int argc, char* argv[])
    : Game(argc, argv)
{
}

void HexaGame::InitGameInfo(GameInfo& outInfo)
{
    outInfo.title = "Hexa";
}
