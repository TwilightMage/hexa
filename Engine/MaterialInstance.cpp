#include "MaterialInstance.h"

#include "Material.h"

void MaterialInstance::set_mesh(const Shared<Mesh>& mesh)
{
    if (mesh_ == mesh) return;
    
    if (auto master = get_material())
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
    
    if (auto master = get_material())
    {
        visible_ = visible;
        master->change_active(shared_from_this());
    }
}

void MaterialInstance::copy_parameters_from(const Shared<MaterialInstance>& another_instance)
{
    auto another_parameters = another_instance->instance_parameters_;
    
    for (auto& parameter : instance_parameters_)
    {
        parameter->value->reset();
        for (auto& another_parameter : another_parameters)
        {
            if (another_parameter->value->name == parameter->value->name)
            {
                if (another_parameter->value->type == parameter->value->type)
                {
                    another_parameter->value->copy(parameter->value.get());
                }
                
                another_parameters.remove(another_parameter->key);
                break;
            }
        }
    }
}

void MaterialInstance::destroy()
{
    if (auto master = get_material())
    {
        master->destroy_instance(shared_from_this());
    }
}

void MaterialInstance::register_direct_parameters()
{
    
}
