#pragma once

#include "ChunkIndex.h"
#include "Engine/Array2D.h"
#include "Engine/Rect.h"

class WorldChunk;
class HexaWorld;

EXTERN class EXPORT WorldChunkObserver
{
    friend HexaWorld;

public:
    void set_render_chunks(bool render_chunks);
    FORCEINLINE Rect get_coverage_rect() const;
    FORCEINLINE Shared<HexaWorld> get_world() const;
    FORCEINLINE const ChunkIndex& get_index() const;
    
private:
    WorldChunkObserver(const ChunkIndex& chunk_index, uint half_size, const Weak<HexaWorld>& world);
    
    ChunkIndex chunk_index_;
    int half_size_;
    Array2D<Shared<WorldChunk>> chunks_;
    Weak<HexaWorld> world_;
    bool render_chunks_;

public:
    void move(const ChunkIndex& new_chunk_index);
};
