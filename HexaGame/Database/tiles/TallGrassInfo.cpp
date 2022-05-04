#include "TallGrassInfo.h"

#include <HexaGame/HexaCollisionMaskBits.h>
#include <OGRE/OgreEntity.h>

#include "Engine/Material.h"
#include "Engine/MeshComponent.h"
#include "Engine/Random.h"
#include "HexaGame/Database/Tiles.h"
#include "HexaGame/Entities/ComplexTile.h"
#include "HexaGame/Worlds/HexaWorld.h"

//void TallGrassInfo::post_loading()
//{
//    material = Game::get_instance()->load_material("Hexa/Foliage")->clone(key.to_string());
//    material->set_texture(texture, 0);
//}

void TallGrassInfo::neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, ConstPtr<TileInfo> new_neighbor) const
{
    if (side == TileSide::Down && new_neighbor == Tiles::air)
    {
        world->set_tile(index, Tiles::air);
    }
}

void TallGrassInfo::setup_spawned_entity(const Shared<ComplexTile>& new_entity, const Shared<ComplexTileCustomData>& custom_data) const
{
    new_entity->set_collision_mask(HexaCollisionMaskBits::COMPLEX_NOBLOCK);
    new_entity->mesh()->set_material_parameter(Quaternion(mesh->get_bounds_half_size().z * 2, 1, 0, 0), 0, 0);
    
    Random random(new_entity->get_index().x + new_entity->get_index().y + new_entity->get_index().z);
    
    const float s_h = random.number(0.9f, 1.1f);
    const float s_v = random.number(0.9f, 1.1f);
    new_entity->set_scale(Vector3(s_h, s_h, s_v));
    new_entity->set_location(new_entity->get_location() + Vector3(
        random.number(-10.f, 10.f),
        random.number(-10.f, 10.f),
        random.number(-10.f, 0.0f)));
}
