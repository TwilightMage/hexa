#pragma once

#include "HexaWorld.h"
#include "HexaGame/Entities/Skybox.h"

class GamePlayer;
class DebugPlayer;

class EXPORT GameWorld : public HexaWorld
{
public:
    explicit GameWorld(const Shared<WorldGenerator>& generator);
    
    void on_start() override;
    void on_tick(float delta_time) override;

private:
    Shared<GamePlayer> player_;
    Shared<Skybox> skybox_;
};
