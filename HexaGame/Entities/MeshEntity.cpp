#include "MeshEntity.h"

MeshEntity::MeshEntity(const Shared<StaticMesh>& mesh)
    : Entity()
    , mesh_(mesh)
{
}

void MeshEntity::on_start()
{
    //set_mesh(mesh_);
}
