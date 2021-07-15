#pragma once

#include "Engine/Entity.h"
#include "HexaGame/ItemContainer.h"
#include "HexaGame/TileIndex.h"

class WorldChunk;
class ItemInfo;

class ItemDrop : public Entity
{
public:
    ItemDrop(const ItemContainer& item);

    void on_start() override;
    void on_tick(float delta_time) override;
    void on_destroy() override;

    FORCEINLINE const ItemContainer& get_item() const { return item_; }

    void set_force(const Vector3& force);

    float offset = 0;

protected:
    bool is_rigid_body() const override { return true; }

private:
    void fixed_tick(const Shared<World>& world);
    void chunk_tile_change(const ChunkIndex& chunk_index, const TileIndex& tile_index);

    Shared<WorldChunk> bound_chunk_;
    
    ItemContainer item_;
    bool is_sleeping_ = true;
    Vector3 fall_force_ = Vector3::zero();

    Vector3 fall_pos_;
    float fixed_tick_accum_ = 0;
    const float fixed_tick_interval_ = 1.0f / 60;
};
