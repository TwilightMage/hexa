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

Image::Image(const Shared<Texture>& texture)
    : UIElement()
    , shader_(Game::get_basic_ui_shader())
    , texture_(texture)
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
    return *texture_;
}

Matrix4x4 Image::get_matrix() const
{
    return get_ui_matrix();
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
    if (*texture_ != new_texture)
    {
        texture_ = new_texture;
        update_uv_rect();
    }
}

void Image::on_register_render()
{
    texture_.activate();
}

void Image::on_unregister_render()
{
    texture_.deactivate();
}

void Image::update_uv_rect()
{
    if (!have_rect_)
    {
        uv_rect_ = Quaternion(1.0f, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        uv_rect_ = Quaternion(rect_.w / static_cast<float>(texture_->get_width()), rect_.h / static_cast<float>(texture_->get_height()), rect_.x / static_cast<float>(texture_->get_width()), rect_.y / static_cast<float>(texture_->get_height()));
    }
}
