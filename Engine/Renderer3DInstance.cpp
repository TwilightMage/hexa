#include "Renderer3DInstance.h"

void Renderer3DInstance::set_model(const Matrix4x4& value) const
{
    model->value = value;
}

void Renderer3DInstance::set_texture(const Shared<Texture>& value) const
{
    texture->value = value;
}


void Renderer3DInstance::register_direct_parameters()
{
    model = get_parameter<Matrix4x4>("model");
    texture = get_parameter<Shared<Texture>>("texture");
}
