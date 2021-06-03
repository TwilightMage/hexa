#include "ChunkMeshEntity.h"

void ChunkMeshEntity::on_start()
{
    make_body_static();
}

bool ChunkMeshEntity::is_rigid_body()
{
    return true;
}
