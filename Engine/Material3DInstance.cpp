#include "Material3DInstance.h"

void Material3DInstance::set_model(const Matrix4x4& value) const
{
    model->value = value;
}

void Material3DInstance::register_direct_parameters()
{
    model = get_parameter<Matrix4x4>("model");
}
