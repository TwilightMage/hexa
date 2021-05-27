#pragma once

#include "HexaGame/HexaWorld.h"

class WorldChunkData;
EXTERN class EXPORT GameWorld : public HexaWorld
{
public:
    explicit GameWorld(const Shared<WorldGenerator>& generator);
    
    void on_start() override;

private:
    void spawn_player(const Shared<WorldChunkData>& sender);
};
