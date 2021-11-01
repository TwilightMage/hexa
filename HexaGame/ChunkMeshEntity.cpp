#include "ChunkMeshEntity.h"

#include "HexaCollisionMaskBits.h"
#include "Engine/MeshComponent.h"

ChunkMeshEntity::ChunkMeshEntity()
    : Entity()
{
    mesh_component_ = create_component<MeshComponent>();
}

void ChunkMeshEntity::on_start()
{
    make_body_static();
    set_collision_mask(HexaCollisionMaskBits::GROUND);
}
