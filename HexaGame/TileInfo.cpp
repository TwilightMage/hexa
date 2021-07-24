#include "TileInfo.h"

#include "HexaGame.h"

void TileInfo::neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, const Shared<const TileInfo>& new_neighbor) const
{
}

void TileInfo::on_tile_break(const TileIndex& index, const Shared<HexaWorld>& world) const
{
}

List<ItemContainer> TileInfo::get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const
{
    return { HexaGame::item_database->get(key) };
}

void ComplexTileInfo::setup_spawned_entity(const Shared<ComplexTile>& new_tile, const Shared<ComplexTileCustomData>& custom_data) const
{
}

void ComplexTileInfo::on_tile_destroyed(const TileIndex& index, const Shared<ComplexTile>& destroyed_tile, const Shared<HexaWorld>& world) const
{
}

bool ComplexTileInfo::can_place_at(const Shared<ComplexTile>& new_tile, const ItemContainer& item) const
{
    return true;
}

Shared<ComplexTileCustomData> ComplexTileInfo::create_custom_data() const
{
    return nullptr;
}
