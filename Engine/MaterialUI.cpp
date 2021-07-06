#include "MaterialUI.h"

#include "MaterialUIInstance.h"

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
