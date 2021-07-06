﻿#include "Material3D.h"

#include "Material3DInstance.h"
#include "MaterialInstance.h"

void Material3D::register_direct_parameters()
{
    view = get_parameter<Matrix4x4>("view");
    projection = get_parameter<Matrix4x4>("projection");
}

void Material3D::apply_params(const RenderData& render_data) const
{
    view->value = render_data.view_3d;
    projection->value = render_data.projection_3d;
}

Shared<MaterialInstance> Material3D::create_instance_object()
{
    return MakeShared<Material3DInstance>();
}
