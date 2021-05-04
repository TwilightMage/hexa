#include "TextureBinding.h"

TextureBinding::TextureBinding(const Shared<Texture>& texture)
    : texture_(texture)
{
}

uint TextureBinding::get_gl_id()
{
    return texture_->get_gl_texture_id();
}

Vector2 TextureBinding::modify_uv(const Vector2& uv)
{
    return uv;
}
