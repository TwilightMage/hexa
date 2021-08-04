#include "MeshEntity.h"

#include "Engine/MeshComponent.h"

MeshEntity::MeshEntity(const Shared<StaticMesh>& mesh)
    : Entity()
    , mesh_(mesh)
{
}

void MeshEntity::on_start()
{
    auto comp = create_component<MeshComponent>();
    comp->set_body_type(PhysicalBodyType::Kinematic);
    comp->set_mesh(mesh_);
}
