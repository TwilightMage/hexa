﻿#pragma once

#include "ChunkIndex.h"
#include "Engine/Tree2D.h"
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

    Shared<WorldChunkObserver> register_chunk_observer(const Rect& rect);
    Shared<WorldChunkObserver> register_chunk_observer(const ChunkIndex& chunk_index, uint half_size);
    
    Shared<WorldChunk> get_chunk(const ChunkIndex& chunk_index);
    
    void dump_observable_area(); 

private:
    void unregister_chunk_observer(WorldChunkObserver* observer);
    void move_observer(WorldChunkObserver* observer, const Rect& new_rect);

    void collect_observer_array(Array2D<Shared<WorldChunk>>& array, int start_x, int start_y);
    void fill_observer_array(Array2D<Shared<WorldChunk>>& array, int start_x, int start_y);
    
    Shared<WorldGenerator> generator_;
    List<Shared<WorldChunkObserver>> chunk_observers_;
};