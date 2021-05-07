﻿#include "Image.h"

#include "Engine/Game.h"
#include "Engine/Texture.h"

Image::Image()
    : UIElement()
    , shader_(Game::get_basic_ui_shader())
    , texture_(Game::get_white_pixel())
    , have_rect_(false)
    , uv_rect_(Quaternion(1.0f, 1.0f, 0.0f, 0.0f))
{
}

Shared<Mesh> Image::get_mesh() const
{
    return Mesh::empty;
}

Shared<Shader> Image::get_shader() const
{
    return shader_;
}

Shared<Texture> Image::get_texture() const
{
    return texture_;
}

Vector3 Image::get_position() const
{
    return get_render_position();
}

Quaternion Image::get_rotation() const
{
    return get_render_rotation();
}

Vector3 Image::get_scale() const
{
    return get_render_scale();
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

void Image::use_shader(const Weak<Shader>& shader)
{
    const auto new_shader = shader.expired() ? Game::get_basic_shader() : shader.lock();
    if (shader_ != new_shader)
    {
        const auto old = shader_;
        shader_ = new_shader;
        
        /*if (auto world_ptr = world_.lock())
        {
            world_ptr->notify_renderable_shader_updated(weak_from_this(), old);
        }*/
    }
}

void Image::use_texture(const Weak<Texture>& texture)
{
    const auto new_texture = texture.expired() ? Game::get_white_pixel() : texture.lock();
    if (texture_ != new_texture)
    {
        if (should_render())
        {
            texture_->usage_count_decrease();
        }
        texture_ = new_texture;
        if (should_render())
        {
            texture_->usage_count_increase();
        }
        update_uv_rect();
    }
}

void Image::on_register_render()
{
    Game::register_ui_element(weak_from_this());
    texture_->usage_count_increase();
}

void Image::on_unregister_render()
{
    Game::unregister_ui_element(weak_from_this());
    texture_->usage_count_decrease();
}

void Image::update_uv_rect()
{
    if (!have_rect_)
    {
        uv_rect_ = Quaternion(1.0f, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        uv_rect_ = Quaternion(rect_.w / (float)texture_->width_, rect_.h / (float)texture_->height_, rect_.x / (float)texture_->width_, rect_.y / (float)texture_->height_);
    }
}