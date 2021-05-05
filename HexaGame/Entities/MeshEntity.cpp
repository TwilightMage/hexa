#include "MeshEntity.h"

MeshEntity::MeshEntity(const Weak<Mesh>& mesh)
    : Entity()
    , mesh_(mesh.lock())
{
}

void MeshEntity::on_start()
{
    use_mesh(mesh_);
}
