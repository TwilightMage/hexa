#include "ChunkMeshEntity.h"

#include "HexaCollisionMaskBits.h"

void ChunkMeshEntity::on_start()
{
    make_body_static();
    use_collision_mask(HexaCollisionMaskBits::GROUND);
}

bool ChunkMeshEntity::is_rigid_body()
{
    return true;
}
