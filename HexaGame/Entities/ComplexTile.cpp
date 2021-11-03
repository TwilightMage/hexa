#include "ComplexTile.h"

#include "Engine/MeshComponent.h"
#include "Engine/Physics/ConcaveMeshCollision.h"
#include "HexaGame/HexaCollisionMaskBits.h"
#include "HexaGame/TileInfo.h"

ComplexTile::ComplexTile(ConstPtr<ComplexTileInfo> tile_info)
    : tile_info_(tile_info)
{
    mesh_component_ = create_component<MeshComponent>();
}

void ComplexTile::on_start()
{
    mesh_component_->set_mesh(tile_info_->mesh, List<Shared<Material>>::generate(tile_info_->mesh->get_material_count(), tile_info_->material));
    
    set_rotation(Quaternion(Vector3(0, 0, 30)));
    set_collision_mask(HexaCollisionMaskBits::COMPLEX_BLOCK);
    make_body_kinematic();

}
