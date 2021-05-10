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
    image_->set_size(get_size());
    add_child(image_);
}

void ButtonImage::on_mouse_enter()
{
    if (is_pressed())
    {
        set_state(1);
    }
    else
    {
        set_state(2);
    }
}

void ButtonImage::on_mouse_leave()
{
    set_state(0);
}

void ButtonImage::on_press()
{
    set_state(1);
    on_click(weak_from_this());
}

void ButtonImage::on_release()
{
    if (is_mouse_over())
    {
        set_state(2);
    }
}

void ButtonImage::set_state(uint state)
{
    state_ = state;
    image_->set_position(Vector2(0.0f, -1.0f * state));
}
