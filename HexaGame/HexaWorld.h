#pragma once

#include "ChunkIndex.h"
#include "Tree2D.h"
#include "Engine/Array2D.h"
#include "Engine/Rect.h"
#include "Engine/World.h"

class WorldChunkObserver;
class WorldChunk;
class WorldGenerator;

EXTERN class EXPORT HexaWorld : public World
{
    friend class WorldChunkObserver;

public:    
    explicit HexaWorld(const Shared<WorldGenerator>& generator);

    const Shared<WorldGenerator>& get_generator() const;
    
    Shared<WorldChunkObserver> register_chunk_observer(const ChunkIndex& chunk_index, uint half_size);
    Shared<WorldChunk> get_chunk(const ChunkIndex& chunk_index);
    
    void dump_observable_area(); 

private:
    void move_observer(WorldChunkObserver* observer, const ChunkIndex& to);

    void fill_observer(WorldChunkObserver* observer);
    
    Shared<WorldGenerator> generator_;
    Tree2D<int, Shared<WorldChunkObserver>> observer_storage_;
};
