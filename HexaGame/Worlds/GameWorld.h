#pragma once

#include "HexaGame/HexaWorld.h"

class GamePlayer;
class DebugPlayer;

EXTERN class EXPORT GameWorld : public HexaWorld
{
public:
    explicit GameWorld(const Shared<WorldGenerator>& generator);
    
    void on_start() override;

private:
    Shared<GamePlayer> player_;
};
