#include "MaterialUI.h"

#include "MaterialUIInstance.h"

bool MaterialUI::verify_shader_valid(const Shared<Shader>& shader)
{
    return shader->check_uniform_presence("model", GLTypeEnum::Mat4, true)
        && shader->check_uniform_presence("uv", GLTypeEnum::Vec4, true)
        && shader->check_uniform_presence("color", GLTypeEnum::Vec4, true)
        && shader->check_uniform_presence("texture", GLTypeEnum::Sampler2D, true)
        && shader->check_uniform_presence("projection", GLTypeEnum::Mat4, false);
}

void MaterialUI::register_direct_parameters()
{
    projection = get_parameter<Matrix4x4>("projection");
}

void MaterialUI::apply_params(const RenderData& render_data) const
{
    projection->value = render_data.projection_2d;
}

Shared<MaterialInstance> MaterialUI::create_instance_object()
{
    return MakeShared<MaterialUIInstance>();
}
