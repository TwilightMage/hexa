#pragma once

#include <array>

#include "Engine/Entity.h"
#include "HexaGame/TileIndex.h"
#include "HexaGame/TileSide.h"

class SolidTileInfo;

class TileDemoEntity : public Entity
{
public:
    TileDemoEntity(const std::array<ConstPtr<SolidTileInfo>, 6>& tiles);

    void on_start() override;

private:
    std::array<ConstPtr<SolidTileInfo>, 6> tiles_;
    inline static const TileIndex tile_positions[6] = {
        TileIndex(0, 0, 0),
        TileIndex(0, 0, 1),
        TileIndex(0, 0, 2),
        TileIndex(0, 1, 0),
        TileIndex(0, 1, 1),
        TileIndex(-1, 0, 0)
    };

    inline static TileSide masks[6] = {
        TileSide::Wall & ~(TileSide::BackRight | TileSide::Back) | TileSide::Down,
        TileSide::Wall & ~TileSide::BackRight,
        TileSide::Wall | TileSide::Up,
        TileSide::Wall & ~(TileSide::BackLeft | TileSide::FrontLeft) | TileSide::Down,
        TileSide::Wall & ~TileSide::FrontLeft | TileSide::Up,
        TileSide::Wall & ~(TileSide::Front | TileSide::FrontRight) | TileSide::Down | TileSide::Up
    };

    Shared<MeshComponent> mesh_component_;
};
