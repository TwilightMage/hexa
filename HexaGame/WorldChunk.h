#pragma once

#include "ChunkIndex.h"
#include "TileIndex.h"
#include "TileSide.h"
#include "TileType.h"
#include "WorldChunkDataState.h"
#include "Engine/Entity.h"

class ComplexTileCustomData;
class ComplexTileInfo;
class ComplexTile;
class WorldChunkObserver;
class WorldGenerator;
class EditableChunk;
class TileInfo;
class HexaWorld;
class WorldChunkMesh;

class EXPORT WorldChunk : public std::enable_shared_from_this<WorldChunk>
{
    friend EditableChunk;
    friend WorldGenerator;
    friend HexaWorld;
    friend WorldChunkObserver;
    
public:
    struct ComplexTileSlot
    {
        Shared<const ComplexTileInfo> info;
        Shared<ComplexTile> entity;
    };
    
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

    void set_tile(const TileIndex& index, const Shared<const TileInfo>& new_tile);

    TileSide get_tile_face_flags(const TileIndex& tile_index) const;

    void save_if_dirty();

    inline static const uint chunk_size = 10;
    inline static const uint chunk_height = 300;

    Delegate<const Shared<WorldChunk>&, WorldChunkDataState> on_state_changed;
    Delegate<const Shared<WorldChunk>&> on_loaded;
    Delegate<const Shared<WorldChunk>&> on_loading;
    Delegate<const ChunkIndex&, const TileIndex&> on_tile_change;

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
    void generate_metadata(uint z);

    void neighbor_tile_changed(const ChunkIndex& chunk, const TileIndex& index);

    void spawn_complex(const TileIndex& local_index, ComplexTileSlot& tile) const;
    bool can_claim(const TileIndex& local_index) const;
    bool claim_tile(const TileIndex& local_index, const Shared<ComplexTile>& claimer);

    public:
    void regenerate_all_complex_tiles();
    void regenerate_whole_mesh(bool fill_complex = false);
    void regenerate_mesh(uint z, bool fill_complex = false);
    void regenerate_cap_mesh();
    void cap(uint z);
    private:
    
    FORCEINLINE Shared<const TileInfo> front_tile(const TileIndex& index) const;
    FORCEINLINE Shared<const TileInfo> front_right_tile(const TileIndex& index) const;
    FORCEINLINE Shared<const TileInfo> back_right_tile(const TileIndex& index) const;
    FORCEINLINE Shared<const TileInfo> back_tile(const TileIndex& index) const;
    FORCEINLINE Shared<const TileInfo> back_left_tile(const TileIndex& index) const;
    FORCEINLINE Shared<const TileInfo> front_left_tile(const TileIndex& index) const;
    FORCEINLINE Shared<const TileInfo> up_tile(const TileIndex& index) const;
    FORCEINLINE Shared<const TileInfo> down_tile(const TileIndex& index) const;

    int observe_counter_ = 0;
    int visibility_counter_ = 0;
    
    int load_counter_ = 0;
    
    ChunkIndex index_;
    Weak<HexaWorld> world_;

    List<List<Shared<Entity>>> mesh_entities_;
    SimpleMap<TileIndex, ComplexTileSlot> complex_tiles_;
    Map<TileIndex, Shared<ComplexTileCustomData>> custom_data_;
    Shared<Entity> cap_entity_;
    Map<TileIndex, Shared<const TileInfo>> modifications_;
    bool dirty_;
    uint cap_z;

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
