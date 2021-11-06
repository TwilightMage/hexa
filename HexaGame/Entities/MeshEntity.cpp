#include "MeshEntity.h"

#include "Engine/Game.h"
#include "Engine/MeshComponent.h"

MeshEntity::MeshEntity(const Shared<StaticMesh>& mesh)
    : Entity()
    , mesh_(mesh)
{
    mesh_component_ = create_component<MeshComponent>();
}

void MeshEntity::on_start()
{
    mesh_component_->set_body_type(PhysicalBodyType::Kinematic);
    mesh_component_->set_mesh(mesh_, Game::get_instance()->load_material("Engine/Basic"));
}
