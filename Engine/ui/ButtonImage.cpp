#include "ButtonImage.h"

#include "Image.h"

ButtonImage::ButtonImage(const Shared<Texture>& texture)
    : texture_(texture)
{
}

void ButtonImage::on_construct()
{
    image_ = MakeShared<Image>();
    image_->use_texture(texture_);
    image_->set_mouse_detection(false);
    image_->set_size(Vector2(static_cast<float>(texture_->get_width()), static_cast<float>(texture_->get_height()) / 3) * 2);
    image_->set_position((get_size() - image_->get_size()) / 2.0f);
    add_child(image_);
    state_updated(get_state());
}

void ButtonImage::on_size_changed()
{
    if (image_)
    {
        image_->set_position((image_->get_size() - get_size()) / 2.0f);
    }
}

void ButtonImage::state_updated(State state)
{
    image_->set_rect(Rect(0, texture_->get_height() / 3 * state, texture_->get_width(), texture_->get_height() / 3));
}
