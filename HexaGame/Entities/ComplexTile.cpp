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
    get_now(t1);
    set_rotation(Quaternion(Vector3(0, 0, 30)));
    get_now(t2);
    set_mesh(tile_info_->mesh);
    get_now(t3);
    set_material(tile_info_->material);
    get_now(t4);
    get_material_instance()->set_param_value("texture", tile_info_->texture);
    get_now(t5);
    set_collision_mask(initial_is_blocking ? HexaCollisionMaskBits::COMPLEX_BLOCK : HexaCollisionMaskBits::COMPLEX_NOBLOCK);
    get_now(t6);
    make_body_kinematic();
    get_now(t7);
    set_collision(MakeShared<ConcaveMeshCollision>(tile_info_->mesh));
    get_now(t8);
    measure_time_all(q, t1, t2, t3, t4, t5, t6, t7, t8);
    auto f = 1;
}
