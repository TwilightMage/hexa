#include "RendererUIInstance.h"

void RendererUIInstance::set_mvp(const Matrix4x4& value) const
{
    mvp->value = value;
}

void RendererUIInstance::set_uv(const Vector2& translate, const Vector2& scale) const
{
    uv->value = Quaternion(translate.x, translate.y, scale.x, scale.y);
}

void RendererUIInstance::set_color(const Color& value) const
{
    color->value = value;
}

void RendererUIInstance::set_texture(const Shared<Texture>& value) const
{
    texture->value = value;
}

void RendererUIInstance::register_direct_parameters()
{
    mvp = get_parameter<Matrix4x4>("mvp");
    uv = get_parameter<Quaternion>("uv");
    color = get_parameter<Color>("color");
    texture = get_parameter<Shared<Texture>>("texture");
}
