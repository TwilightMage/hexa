#pragma once

#include "HexaGame/ChunkIndex.h"
#include "HexaGame/TileIndex.h"
#include "Engine/Tree2D.h"
#include "Engine/Array2D.h"
#include "Engine/Rect.h"
#include "Engine/World.h"
#include "HexaGame/ItemContainer.h"
#include "HexaGame/PathConfig.h"

class WorldPath;
class Character;
class TileInfo;
class WorldChunkObserver;
class WorldChunk;
class WorldGenerator;

class EXPORT HexaWorld : public World
{
    friend class WorldChunkObserver;

public:    
    explicit HexaWorld(const Shared<WorldGenerator>& generator);

    const Shared<WorldGenerator>& get_generator() const;

    Shared<WorldChunkObserver> register_chunk_observer(const Rect& rect);
    Shared<WorldChunkObserver> register_chunk_observer(const ChunkIndex& chunk_index, uint half_size);
    
    Shared<WorldChunk> get_chunk(const ChunkIndex& chunk_index) const;

    Shared<WorldPath> FindPath(const PathConfig& config);

    bool spawn_character(const Shared<Character>& character, const TileIndex& world_index);
    Shared<Character> get_character_at(const TileIndex& world_index) const;
    Shared<Entity> spawn_drop(const TileIndex& tile, const ItemContainer& item);

    void set_tile(const TileIndex& index, const Shared<const TileInfo>& id) const;
    Shared<const TileInfo> get_tile_id(const TileIndex& index) const;

    bool damage_tile(const TileIndex& index, float damage) const;

    void cap_chunks(uint z);
    void uncap_chunks();
    FORCEINLINE uint get_cap_chunk_z() const { return cap_chunk_z; }
    
    void dump_observable_area();

    Delegate<const Shared<Character>&> on_character_spawned;

protected:

private:
    void character_destroyed_callback(const Shared<Entity>& character);
    
    Shared<WorldChunk> get_chunk_internal(const ChunkIndex& chunk_index) const;
    
    void unregister_chunk_observer(WorldChunkObserver* observer);
    void move_observer(WorldChunkObserver* observer, const Rect& new_rect);

    void collect_observer_array(Array2D<Shared<WorldChunk>>& array, int start_x, int start_y);
    void fill_observer_array(Array2D<Shared<WorldChunk>>& array, int start_x, int start_y);
    
    Shared<WorldGenerator> generator_;
    List<Shared<WorldChunkObserver>> chunk_observers_;

    List<Shared<Character>> characters_;

    uint cap_chunk_z;
};
