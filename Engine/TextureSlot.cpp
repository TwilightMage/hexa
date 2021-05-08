#include "TextureSlot.h"

#include "Texture.h"

#define INC_REF()\
    if (texture_ && is_active_)\
    {\
        texture_->usage_count_increase();\
    }\

#define DEC_REF()\
    if (texture_ && is_active_)\
    {\
        texture_->usage_count_decrease();\
    }\

TextureSlot::TextureSlot()
{
}

TextureSlot::TextureSlot(const TextureSlot& rhs)
    : texture_(rhs.texture_)
    , is_active_(rhs.is_active_)
{
    INC_REF();
}

TextureSlot::TextureSlot(const Shared<Texture>& rhs)
    : texture_(rhs)
    , is_active_(true)
{
    INC_REF();
}

TextureSlot::TextureSlot(bool state)
    : is_active_(state)
{
}

TextureSlot::TextureSlot(const Shared<Texture>& texture, bool state)
    : texture_(texture)
    , is_active_(state)
{
}

TextureSlot::~TextureSlot()
{
    DEC_REF();
}

void TextureSlot::activate()
{
    if (!is_active_)
    {
        is_active_ = true;
    
        if (texture_)
        {
            texture_->usage_count_increase();
        }
    }
}

void TextureSlot::deactivate()
{
    if (is_active_)
    {
        is_active_ = false;
    
        if (texture_)
        {
            texture_->usage_count_decrease();
        }
    }
}

TextureSlot& TextureSlot::operator=(const TextureSlot& rhs)
{
    if (texture_ != rhs.texture_)
    {
        DEC_REF();
        texture_ = rhs.texture_;
        is_active_ = rhs.is_active_;
        INC_REF();
    }
    else
    {
        if (is_active_ != rhs.is_active_)
        {
            is_active_ = rhs.is_active_;
            if (is_active_)
            {
                activate();
            }
            else
            {
                deactivate();
            }
        }
    }
    return *this;
}

const Shared<Texture>& TextureSlot::operator->() const
{
    return texture_;
}

const Shared<Texture>& TextureSlot::operator*() const
{
    return texture_;
}
