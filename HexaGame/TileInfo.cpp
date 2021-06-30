#include "TileInfo.h"

#include "HexaGame.h"

void TileInfo::neighbor_changed(const TileIndex& index, TileSide side, const Shared<World>& world, const Shared<const TileInfo>& new_neighbor) const
{
}

List<ItemContainer> TileInfo::get_drops(const TileIndex& index, const Shared<World>& world) const
{
    return { HexaGame::item_database->get(key) };
}
