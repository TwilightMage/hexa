#include "pch.h"

#include "HexaGame/HexaGame.h"

int main(int argc, char* argv[])
{
    HexaGame* game = new HexaGame(argc, argv);
    game->launch();
    
    return 0;
}
