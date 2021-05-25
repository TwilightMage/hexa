#pragma once

#include "TileIndex.h"
#include "TileSide.h"
#include "TileType.h"
#include "Engine/BasicTypes.h"

class WorldChunkMesh;
class TileInfo;

class WorldChunkData
{
public:
    inline static const uint chunk_size = 10;
    inline static const uint chunk_height = 300;
    
    void validate_and_generate_metadata();
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
    
    Shared<const TileInfo> data[chunk_size][chunk_size][chunk_height];
    TileType plane_metadata[chunk_height];

private:
    WorldChunkData* front_ = nullptr;
    WorldChunkData* right_ = nullptr;
    WorldChunkData* front_right_ = nullptr;
    WorldChunkData* back_ = nullptr;
    WorldChunkData* left_ = nullptr;
    WorldChunkData* back_left_ = nullptr;
};
