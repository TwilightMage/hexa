#include "TallGrassInfo.h"

#include "Engine/Random.h"
#include "HexaGame/Database/Tiles.h"
#include "HexaGame/Entities/ComplexTile.h"
#include "HexaGame/Worlds/HexaWorld.h"

void TallGrassInfo::neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, const Shared<const TileInfo>& new_neighbor) const
{
    if (side == TileSide::Down && new_neighbor == Tiles::air)
    {
        world->set_tile(index, Tiles::air);
    }
}

void TallGrassInfo::setup_spawned_entity(const Shared<ComplexTile>& new_entity, const Shared<ComplexTileCustomData>& custom_data) const
{
    new_entity->get_material_instance()->set_param_value("height", mesh->get_bounds_half_size().z * 2);
    
    Random random(new_entity->get_index().x + new_entity->get_index().y + new_entity->get_index().z);
    
    const float s_h = random.number(0.9f, 1.1f);
    const float s_v = random.number(0.9f, 1.1f);
    new_entity->set_scale(Vector3(s_h, s_h, s_v));
    new_entity->set_position(new_entity->get_position() + Vector3(
        random.number(-0.1f, 0.1f),
        random.number(-0.1f, 0.1f),
        0));
}
