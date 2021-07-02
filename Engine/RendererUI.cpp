#include "RendererUI.h"

#include "RendererUIInstance.h"

bool RendererUI::verify_shader_valid(const Shared<Shader>& shader)
{
    return shader->check_uniform_presence("model", GLTypeEnum::Mat4, true)
        && shader->check_uniform_presence("uv", GLTypeEnum::Vec4, true)
        && shader->check_uniform_presence("color", GLTypeEnum::Vec4, true)
        && shader->check_uniform_presence("texture", GLTypeEnum::Sampler2D, true)
        && shader->check_uniform_presence("projection", GLTypeEnum::Mat4, false);
}

void RendererUI::register_direct_parameters()
{
    projection = get_parameter<Matrix4x4>("projection");
}

void RendererUI::apply_params(const RenderData& render_data) const
{
    projection->value = render_data.projection_2d;
}

Shared<RendererInstance> RendererUI::create_instance_object()
{
    return MakeShared<RendererUIInstance>();
}
