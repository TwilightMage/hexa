#pragma once

#include "HexaGame/HexaWorld.h"

class DebugPlayer;

EXTERN class EXPORT GameWorld : public HexaWorld
{
public:
    explicit GameWorld(const Shared<WorldGenerator>& generator);
    
    void on_start() override;

private:
    void spawn_chunk_loaded(const Shared<WorldChunk>& sender);
    
    Shared<DebugPlayer> player_;
};
