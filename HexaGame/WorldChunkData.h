#pragma once

#include "ChunkIndex.h"
#include "TileIndex.h"
#include "TileSide.h"
#include "TileType.h"
#include "WorldChunkDataState.h"
#include "Engine/BasicTypes.h"
#include "Engine/Delegate.h"

class WorldGenerator;
class EditableChunk;
class WorldChunkMesh;
class TileInfo;

EXTERN class EXPORT WorldChunkData : public std::enable_shared_from_this<WorldChunkData>
{
    friend EditableChunk;
    friend WorldGenerator;
    
public:
    explicit WorldChunkData(const ChunkIndex& index);
    
    inline static const uint chunk_size = 10;
    inline static const uint chunk_height = 300;
    
    void generate_metadata();
    Shared<WorldChunkMesh> generate_mesh() const;
    
    TileSide get_tile_face_flags(const TileIndex& tile_index) const;
    
    const Shared<const TileInfo>& front_tile(const TileIndex& index) const;
    const Shared<const TileInfo>& front_right_tile(const TileIndex& index) const;
    const Shared<const TileInfo>& back_right_tile(const TileIndex& index) const;
    const Shared<const TileInfo>& back_tile(const TileIndex& index) const;
    const Shared<const TileInfo>& back_left_tile(const TileIndex& index) const;
    const Shared<const TileInfo>& front_left_tile(const TileIndex& index) const;
    const Shared<const TileInfo>& up_tile(const TileIndex& index) const;
    const Shared<const TileInfo>& down_tile(const TileIndex& index) const;

    void link(WorldChunkData* front, WorldChunkData* right, WorldChunkData* front_right, WorldChunkData* back, WorldChunkData* left, WorldChunkData* back_left);
    void unlink();

    const ChunkIndex& get_index() const;
    WorldChunkDataState get_state() const;
    Delegate<const Shared<WorldChunkData>&, WorldChunkDataState> on_state_changed;
    Delegate<const Shared<WorldChunkData>&> on_loaded;
    Delegate<const Shared<WorldChunkData>&> on_loading;
    const Shared<const TileInfo>& get_tile(const TileIndex& index) const;

private:
    void set_state(WorldChunkDataState state);
    
    ChunkIndex index_;
    WorldChunkDataState state_;

    Shared<const TileInfo> data[chunk_size][chunk_size][chunk_height];
    TileType plane_metadata[chunk_height];
    
    WorldChunkData* front_ = nullptr;
    WorldChunkData* right_ = nullptr;
    WorldChunkData* front_right_ = nullptr;
    WorldChunkData* back_ = nullptr;
    WorldChunkData* left_ = nullptr;
    WorldChunkData* back_left_ = nullptr;
};
