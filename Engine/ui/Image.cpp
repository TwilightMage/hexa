#include "Image.h"

#include "Engine/Game.h"
#include "Engine/Texture.h"

Image::Image()
    : UIElement()
    , texture_(nullptr)
    , have_rect_(false)
    , uv_rect_(Quaternion(0.0f, 0.0f, 1.0f, 1.0f))
{
    
}

Image::Image(const Shared<Texture>& texture)
    : UIElement()
    , texture_(texture)
    , have_rect_(false)
    , uv_rect_(Quaternion(0.0f, 0.0f, 1.0f, 1.0f))
{

}

Shared<Texture> Image::get_texture() const
{
    return texture_;
}

Quaternion Image::get_uv_rect() const
{
    return uv_rect_;
}

const Rect& Image::get_rect() const
{
    return rect_;
}

void Image::set_rect(const Rect& rect)
{
    rect_ = rect;
    have_rect_ = true;
    update_uv_rect();
}

void Image::clear_rect()
{
    have_rect_ = false;
    update_uv_rect();
}

void Image::use_texture(const Shared<Texture>& texture)
{
    if (texture_ == texture) return;
    
    texture_ = texture;
    update_uv_rect();
}

void Image::on_register_render()
{
}

void Image::on_unregister_render()
{
}

void Image::matrix_updated(const Matrix4x4& matrix)
{
}

void Image::color_updated(const Color& color)
{
}

void Image::update_uv_rect()
{
    if (!have_rect_)
    {
        uv_rect_ = Quaternion(0.0f, 0.0f, 1.0f, 1.0f);
    }
    else
    {
        uv_rect_ = Quaternion(rect_.x / static_cast<float>(texture_->get_width()), rect_.y / static_cast<float>(texture_->get_height()), rect_.w / static_cast<float>(texture_->get_width()), rect_.h / static_cast<float>(texture_->get_height()));
    }
}
