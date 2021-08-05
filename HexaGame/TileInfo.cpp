#include "TileInfo.h"

#include <OGRE/Main/OgreTechnique.h>

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
    material = MakeShared<Material>();
    material->ogre_material_ = Game::get_instance()->get_material("Hexa/Tile")->ogre_material_->clone((String("Hexa/Tile/") + key.to_string()).c());
    material->ogre_material_->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName((key.to_string() + ".png").c());
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
