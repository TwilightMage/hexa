#include "ButtonImage.h"

#include "Image.h"

ButtonImage::ButtonImage()
{
}

ButtonImage::ButtonImage(const Shared<Texture>& texture)
    : texture_(texture)
{
}

void ButtonImage::set_texture(const Shared<Texture>& texture)
{
    texture_ = texture;
    image_->use_texture(texture);
    state_updated(get_state());
}

void ButtonImage::set_stretch_image(bool value)
{
    stretch_image_ = value;
    on_size_changed();
}

void ButtonImage::on_construct()
{
    image_ = MakeShared<Image>();
    if (texture_)
    {
        image_->use_texture(texture_);
    }
    image_->set_mouse_detection(false);
    image_->set_position((get_size() - image_->get_size()) / 2.0f);
    add_child(image_);
    
    state_updated(get_state());
    on_size_changed();
}

void ButtonImage::on_size_changed()
{
    if (image_)
    {
        if (stretch_image_ || texture_ == nullptr)
        {
            image_->set_size(get_size());
        }
        else
        {
            image_->set_size(Vector2(static_cast<float>(texture_->get_width()), static_cast<float>(texture_->get_height()) / 3));
        }
        
        image_->set_position((image_->get_size() - get_size()) / 2.0f);
    }
}

void ButtonImage::state_updated(State state)
{
    if (is_started_construction() && texture_)
    {
        image_->set_rect(Rect(0, texture_->get_height() / 3 * state, texture_->get_width(), texture_->get_height() / 3));
    }
}
