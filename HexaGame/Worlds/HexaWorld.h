#pragma once

#include "HexaGame/ChunkIndex.h"
#include "HexaGame/TileIndex.h"
#include "Engine/Array2D.h"
#include "Engine/Rect.h"
#include "Engine/World.h"
#include "HexaGame/ItemContainer.h"
#include "HexaGame/PathConfig.h"

class NavNode;
class NavConnection;
class ComplexTileCustomData;
class ItemDrop;
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
    Shared<ItemDrop> spawn_drop(const TileIndex& tile, const ItemContainer& item, const Vector3& throw_force = Vector3(0, 0, 0.2f));

    void set_tile(const TileIndex& world_index, const Shared<const TileInfo>& id) const;
    Shared<const TileInfo> get_tile_id(const TileIndex& world_index) const;

    void break_tile(const TileIndex& world_index) const;
    bool damage_tile(const TileIndex& world_index, float damage) const;
    Shared<ComplexTileCustomData> get_custom_data(const TileIndex& world_index);

    const NavNode* get_nav_node(const TileIndex& world_index) const;
    SimpleMap<TileIndex, NavConnection*> get_nav_connections(const TileIndex& world_index) const;

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
