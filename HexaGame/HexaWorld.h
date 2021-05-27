#pragma once

#include "ChunkIndex.h"
#include "Tree2D.h"
#include "Engine/Array2D.h"
#include "Engine/Rect.h"
#include "Engine/World.h"

class WorldChunk;
class WorldGenerator;

EXTERN class EXPORT HexaWorld : public World
{
    friend class ChunkObserver;

public:
    class ChunkObserver
    {
        friend HexaWorld;

    public:
        void set_render_chunks(bool render_chunks);
        Rect get_coverage_rect() const;
    
    private:
        ChunkObserver(const ChunkIndex& chunk_index, uint half_size, const Weak<HexaWorld>& world);
    
        ChunkIndex chunk_index_;
        int half_size_;
        Array2D<Shared<WorldChunk>> chunks_;
        Weak<HexaWorld> world_;
        bool render_chunks_;

    public:
        void move(const ChunkIndex& chunk_index);
    };
    
    explicit HexaWorld(const Shared<WorldGenerator>& generator);

    const Shared<WorldGenerator>& get_generator() const;
    
    Shared<ChunkObserver> register_chunk_observer(const ChunkIndex& chunk_index, uint half_size);
    Shared<WorldChunk> get_chunk(const ChunkIndex& chunk_index);
    
    void dump_observable_area(); 

private:
    Shared<WorldChunk> generate_chunk_callback(const ChunkIndex& tile_index);

    void move_observer(ChunkObserver* observer, const ChunkIndex& to);
    
    Shared<WorldGenerator> generator_;
    Tree2D<int, Shared<ChunkObserver>> chunk_storage_;
};
