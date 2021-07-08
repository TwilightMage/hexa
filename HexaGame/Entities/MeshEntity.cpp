#include "MeshEntity.h"

MeshEntity::MeshEntity(const Shared<Mesh>& mesh)
    : Entity()
    , mesh_(mesh)
{
}

void MeshEntity::on_start()
{
    set_mesh(mesh_);
}
