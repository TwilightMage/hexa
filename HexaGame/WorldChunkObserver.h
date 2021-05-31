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
    ~WorldChunkObserver();
    
    void set_render_chunks(bool render_chunks);
    FORCEINLINE const Rect& get_rect() const;
    FORCEINLINE Shared<HexaWorld> get_world() const;
    
private:
    WorldChunkObserver(const Rect& rect, const Weak<HexaWorld>& world);
    
    Rect rect_;
    Array2D<Shared<WorldChunk>> chunks_;
    Weak<HexaWorld> world_;
    bool render_chunks_;

public:
    void move(const Rect& rect);
    void move(const ChunkIndex& new_chunk_index, uint half_size);
};
