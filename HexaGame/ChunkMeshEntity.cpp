#include "ChunkMeshEntity.h"

void ChunkMeshEntity::on_start()
{
    set_gravity_enabled(false);
}

bool ChunkMeshEntity::is_rigid_body()
{
    return true;
}
