#include "Image.h"

#include "Engine/Game.h"
#include "Engine/RendererUI.h"
#include "Engine/RendererUIInstance.h"
#include "Engine/Texture.h"

Image::Image()
    : UIElement()
    , renderer_instance_(cast<RendererUIInstance>(Game::get_basic_renderer_ui()->create_instance()))
    , texture_(Game::get_white_pixel(), false)
    , have_rect_(false)
    , uv_rect_(Quaternion(0.0f, 0.0f, 1.0f, 1.0f))
{
}

Image::Image(const Shared<Texture>& texture)
    : UIElement()
    , renderer_instance_(cast<RendererUIInstance>(Game::get_basic_renderer_ui()->create_instance()))
    , texture_(texture, false)
    , have_rect_(false)
    , uv_rect_(Quaternion(0.0f, 0.0f, 1.0f, 1.0f))
{
}

Shared<Texture> Image::get_texture() const
{
    return *texture_;
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

void Image::use_shader(const Shared<Shader>& shader)
{
    if (renderer_instance_ != shader)
    {
        const auto old = renderer_instance_;
        renderer_instance_ = shader;
        
        /*if (auto world_ptr = world_.lock())
        {
            world_ptr->notify_renderable_shader_updated(weak_from_this(), old);
        }*/
    }
}

void Image::use_texture(const Shared<Texture>& texture)
{
    if (*texture_ != texture)
    {
        texture_ = texture;
        update_uv_rect();
    }
}

void Image::on_register_render()
{
    renderer_instance_->set_param("texture")
}

void Image::on_unregister_render()
{
    texture_.deactivate();
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
