#include "MaterialInstance.h"

#include "Material.h"

void MaterialInstance::set_mesh(const Shared<Mesh>& mesh)
{
    if (mesh_ == mesh) return;
    
    if (auto master = get_master_material())
    {
        if (master->shader_->get_empty_vertex() > 0) return;
        
        const auto old_mesh = mesh_;
        mesh_ = mesh;
        master->change_mesh(shared_from_this(), old_mesh, mesh);
    }
}

void MaterialInstance::set_visible(bool visible)
{
    if (visible_ == visible) return;
    
    if (auto master = get_master_material())
    {
        visible_ = visible;
        master->change_active(shared_from_this());
    }
}

void MaterialInstance::destroy()
{
    if (auto master = get_master_material())
    {
        master->destroy_instance(shared_from_this());
    }
}

void MaterialInstance::register_direct_parameters()
{
    
}
