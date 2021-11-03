#pragma once

#include "HexaWorld.h"

class Player;

class EXPORT GameWorld : public HexaWorld
{
public:
    explicit GameWorld(const Shared<WorldGenerator>& generator);
    
    void on_start() override;

private:
    Shared<Player> player_;
};
