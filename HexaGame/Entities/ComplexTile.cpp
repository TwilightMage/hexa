#include "ComplexTile.h"

#include "Engine/performance.h"
#include "Engine/Random.h"
#include "Engine/Physics/ConcaveMeshCollision.h"
#include "HexaGame/HexaCollisionMaskBits.h"
#include "HexaGame/TileInfo.h"

ComplexTile::ComplexTile(const Shared<const ComplexTileInfo>& tile_info)
    : tile_info_(tile_info)
{
}

void ComplexTile::on_start()
{
    set_rotation(Quaternion(Vector3(0, 0, 30)));
    set_mesh(tile_info_->mesh);
    set_material(tile_info_->material);
    get_material_instance()->set_param_value("texture", tile_info_->texture);
    set_collision_mask(initial_is_blocking ? HexaCollisionMaskBits::COMPLEX_BLOCK : HexaCollisionMaskBits::COMPLEX_NOBLOCK);
    make_body_kinematic();
    set_collision(MakeShared<ConcaveMeshCollision>(tile_info_->mesh));

}
