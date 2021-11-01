#pragma once

#include "Engine/Entity.h"

class WorldChunk;

class ChunkMeshEntity : public Entity
{
    friend WorldChunk;

public:
    ChunkMeshEntity();
    
    void on_start() override;

protected:
    Shared<MeshComponent> mesh_component_;
};
