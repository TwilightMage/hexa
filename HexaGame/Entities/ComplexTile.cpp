#include "ComplexTile.h"

#include "Engine/Physics/ConcaveMeshCollision.h"
#include "HexaGame/HexaCollisionMaskBits.h"
#include "HexaGame/TileInfo.h"

ComplexTile::ComplexTile(const Shared<const ComplexTileInfo>& tile_info)
    : tile_info_(tile_info)
{
}

void ComplexTile::on_start()
{
    /*set_rotation(Quaternion(Vector3(0, 0, 30)));
    set_collision_mask(HexaCollisionMaskBits::COMPLEX_BLOCK);
    make_body_kinematic();
    set_collision(MakeShared<ConcaveMeshCollision>(tile_info_->mesh));*/

}
