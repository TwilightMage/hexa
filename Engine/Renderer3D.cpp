#include "Renderer3D.h"

#include "Renderer3DInstance.h"
#include "RendererInstance.h"

bool Renderer3D::verify_shader_valid(const Shared<Shader>& shader)
{
    return shader->check_uniform_presence("model", GLTypeEnum::Mat4, true)
        && shader->check_uniform_presence("texture", GLTypeEnum::Sampler2D, true)
        && shader->check_uniform_presence("view", GLTypeEnum::Mat4, false)
        && shader->check_uniform_presence("projection", GLTypeEnum::Mat4, false);
}

void Renderer3D::register_direct_parameters()
{
    view = get_parameter<Matrix4x4>("view");
    projection = get_parameter<Matrix4x4>("projection");
}

void Renderer3D::apply_params(const RenderData& render_data) const
{
    view->value = render_data.view_3d;
    projection->value = render_data.projection_3d;
}

Shared<RendererInstance> Renderer3D::create_instance_object()
{
    return MakeShared<Renderer3DInstance>();
}
