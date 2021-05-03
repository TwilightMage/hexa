#pragma once

#include "ITextureBinding.h"
#include "Texture.h"

class TextureBinding : public ITextureBinding
{
public:
    TextureBinding(const Shared<Texture>& texture);
    
    uint get_gl_id() override;
    Vector2 modify_uv(const Vector2& uv) override;

private:
    Shared<Texture> texture_;
};
