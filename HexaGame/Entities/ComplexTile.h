#pragma once

#include "Engine/Entity.h"
#include "HexaGame/TileIndex.h"

class ComplexTileCustomData;
class WorldChunk;
class ComplexTileInfo;

class ComplexTile : public Entity
{
    friend WorldChunk;

public:
    ComplexTile(ConstPtr<ComplexTileInfo> tile_info);

    void on_start() override;

    FORCEINLINE const TileIndex& get_index() const { return index_; }

protected:
    bool initial_is_blocking = true;

private:
    Shared<MeshComponent> mesh_component_;
    ConstPtr<ComplexTileInfo> tile_info_;
    TileIndex index_;
};
