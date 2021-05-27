#pragma once

#include "ChunkIndex.h"
#include "WorldChunkDataState.h"
#include "Engine/Entity.h"

class HexaWorld;
class WorldChunkMesh;
class WorldChunkData;

EXTERN class EXPORT WorldChunk
{
public:
    WorldChunk(const ChunkIndex& index, const Weak<HexaWorld>& world);

    void load();
    void set_visibility(bool visibility);

    FORCEINLINE const ChunkIndex& get_index() const { return index_; }
    FORCEINLINE const Shared<WorldChunkData>& get_data() const { return data_; }

private:
    void appear_mesh();
    void delay_mesh_appear(const Shared<WorldChunkData>& sender);
    
    int visibility_counter_ = 0;
    
    ChunkIndex index_;
    Weak<HexaWorld> world_;
    
    Shared<WorldChunkData> data_;
    Shared<WorldChunkMesh> chunk_mesh_;

    List<Shared<Entity>> child_domains_;
};
