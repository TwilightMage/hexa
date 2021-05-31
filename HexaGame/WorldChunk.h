#pragma once

#include "ChunkIndex.h"
#include "TileIndex.h"
#include "TileSide.h"
#include "TileType.h"
#include "WorldChunkDataState.h"
#include "Engine/Entity.h"

class WorldChunkObserver;
class WorldGenerator;
class EditableChunk;
class TileInfo;
class HexaWorld;
class WorldChunkMesh;

EXTERN class EXPORT WorldChunk : public std::enable_shared_from_this<WorldChunk>
{
    friend EditableChunk;
    friend WorldGenerator;
    friend HexaWorld;
    friend WorldChunkObserver;
    
public:
    WorldChunk(const ChunkIndex& index, const Weak<HexaWorld>& world);
    
    FORCEINLINE int get_observe_counter() const;
    FORCEINLINE int get_visibility_counter() const;

    FORCEINLINE WorldChunk* get_front() const;
    FORCEINLINE WorldChunk* get_front_right() const;
    FORCEINLINE WorldChunk* get_right() const;
    FORCEINLINE WorldChunk* get_back() const;
    FORCEINLINE WorldChunk* get_back_left() const;
    FORCEINLINE WorldChunk* get_left() const;

    FORCEINLINE const ChunkIndex& get_index() const;
    FORCEINLINE WorldChunkDataState get_state() const;
    FORCEINLINE const Shared<const TileInfo>& get_tile(const TileIndex& index) const;

    TileSide get_tile_face_flags(const TileIndex& tile_index) const;

    inline static const uint chunk_size = 10;
    inline static const uint chunk_height = 300;

    Delegate<const Shared<WorldChunk>&, WorldChunkDataState> on_state_changed;
    Delegate<const Shared<WorldChunk>&> on_loaded;
    Delegate<const Shared<WorldChunk>&> on_loading;

private:
    void load();
    
    void try_show_mesh();
    void loaded_callback(const Shared<WorldChunk>& sender);
    
    void set_state(WorldChunkDataState state);

    void link(WorldChunk* front, WorldChunk* right, WorldChunk* front_right, WorldChunk* back, WorldChunk* left, WorldChunk* back_left);
    void unlink();

    bool inc_observe();
    bool dec_observe();

    void inc_visibility();
    void dec_visibility();

    void generate_metadata();

    void regenerate_mesh();

    FORCEINLINE const Shared<const TileInfo>& front_tile(const TileIndex& index) const;
    FORCEINLINE const Shared<const TileInfo>& front_right_tile(const TileIndex& index) const;
    FORCEINLINE const Shared<const TileInfo>& back_right_tile(const TileIndex& index) const;
    FORCEINLINE const Shared<const TileInfo>& back_tile(const TileIndex& index) const;
    FORCEINLINE const Shared<const TileInfo>& back_left_tile(const TileIndex& index) const;
    FORCEINLINE const Shared<const TileInfo>& front_left_tile(const TileIndex& index) const;
    FORCEINLINE const Shared<const TileInfo>& up_tile(const TileIndex& index) const;
    FORCEINLINE const Shared<const TileInfo>& down_tile(const TileIndex& index) const;

    int observe_counter_ = 0;
    int visibility_counter_ = 0;
    
    int load_counter_ = 0;
    
    ChunkIndex index_;
    Weak<HexaWorld> world_;

    List<Shared<Entity>> mesh_entities_;

    WorldChunkDataState state_;

    Shared<const TileInfo> data[chunk_size][chunk_size][chunk_height];
    TileType plane_metadata[chunk_height];
    
    WorldChunk* front_ = nullptr;
    WorldChunk* right_ = nullptr;
    WorldChunk* front_right_ = nullptr;
    WorldChunk* back_ = nullptr;
    WorldChunk* left_ = nullptr;
    WorldChunk* back_left_ = nullptr;
};
