#include "TileInfo.h"

#include "HexaGame.h"
#include "Engine/Material.h"

TileInfo::TileInfo(
    const Name& key,
    TileType type,
    const Set<Name> tags,
    float hardness,
    bool block_nav
    )
    : DatabaseEntry(key)
    , type(type)
    , tags(tags)
    , hardness(hardness)
    , block_nav(block_nav)
{
    material = Game::get_instance()->get_material("Hexa/Basic")->clone(("Hexa/Tiles/" + key.to_string()).c());
    material->set_texture(key.to_string() + ".png", 0);
}

void TileInfo::neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, ConstPtr<TileInfo> new_neighbor) const
{
}

void TileInfo::on_tile_break(const TileIndex& index, const Shared<HexaWorld>& world) const
{
}

List<ItemContainer> TileInfo::get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const
{
    return { HexaGame::item_database->get(key) };
}

void ComplexTileInfo::post_loading()
{
    if (material == nullptr)
    {
        material = Game::get_instance()->get_material("Hexa/BasicComplex")->clone(key.to_string());
        material->set_texture(key.to_string() + ".png", 0);
    }
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
