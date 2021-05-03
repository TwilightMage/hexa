#include "TextureAtlasBinding.h"

TextureAtlasBinding::TextureAtlasBinding(const Shared<TextureAtlas>& texture_atlas, uint entry_id)
    : texture_atlas_(texture_atlas)
    , entry_id_(entry_id)
{
}

uint TextureAtlasBinding::get_gl_id()
{
    return texture_atlas_->get_gl_id();
}

Vector2 TextureAtlasBinding::modify_uv(const Vector2& uv)
{
    if (auto entry = texture_atlas_->get_entry(entry_id_))
    {
        return uv * entry->get_scale() + entry->get_offset();
    }
    
    return uv; 
}
