#pragma once

#include "Engine/Entity.h"

class WorldChunkMesh;
class WorldChunkData;

class WorldChunk : public Entity
{
public:
    WorldChunk(const Shared<WorldChunkData>& data);

    void make_visible();

private:
    Shared<WorldChunkData> data_;
    Shared<WorldChunkMesh> chunk_mesh_;

    List<Shared<Entity>> child_domains_;
};
