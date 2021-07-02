#include "RendererInstance.h"

#include "Renderer.h"

void RendererInstance::set_mesh(const Shared<Mesh>& mesh)
{
    if (mesh_ == mesh) return;
    
    if (auto master = get_master_renderer())
    {
        if (master->shader_->get_empty_vertex() > 0) return;
        
        const auto old_mesh = mesh_;
        mesh_ = mesh;
        master->change_mesh(shared_from_this(), old_mesh, mesh);
    }
}

void RendererInstance::set_visible(bool visible)
{
    if (visible_ == visible) return;
    
    if (auto master = get_master_renderer())
    {
        visible_ = visible;
        master->change_active(shared_from_this());
    }
}

void RendererInstance::destroy()
{
    if (auto master = get_master_renderer())
    {
        master->destroy_instance(shared_from_this());
    }
}

void RendererInstance::register_direct_parameters()
{
    
}
