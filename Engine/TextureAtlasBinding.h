#pragma once

#include "BasicTypes.h"
#include "ITextureBinding.h"
#include "TextureAtlas.h"

class TextureAtlasBinding : public ITextureBinding
{
public:
    TextureAtlasBinding(const Shared<TextureAtlas>& texture_atlas, uint entry_id);

    uint get_gl_id() override;
    Vector2 modify_uv(const Vector2& uv) override;

private:
    Shared<TextureAtlas> texture_atlas_;
    uint entry_id_;
};
