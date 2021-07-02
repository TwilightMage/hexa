#include "Image.h"

#include "Engine/Game.h"
#include "Engine/RendererUI.h"
#include "Engine/RendererUIInstance.h"
#include "Engine/Texture.h"

Image::Image()
    : UIElement()
    , renderer_(cast<RendererUI>(Game::get_basic_renderer_ui()))
    , texture_(Game::get_white_pixel())
    , have_rect_(false)
    , uv_rect_(Quaternion(0.0f, 0.0f, 1.0f, 1.0f))
{
}

Image::Image(const Shared<Texture>& texture)
    : UIElement()
    , renderer_(cast<RendererUI>(Game::get_basic_renderer_ui()))
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

    if (renderer_instance_) renderer_instance_->set_texture(texture_);
}

void Image::set_renderer(const Shared<Renderer>& renderer)
{
    if (renderer_instance_)
    {
        renderer_instance_->destroy();
    }

    renderer_ = cast<RendererUI>(renderer);

    on_register_render();
}

Shared<Renderer> Image::get_renderer() const
{
    return renderer_;
}

void Image::on_register_render()
{
    if (renderer_)
    {
        if (renderer_instance_ = cast<RendererUIInstance>(renderer_->create_instance()))
        {
            renderer_instance_->set_color(get_color());
            renderer_instance_->set_mvp(get_ui_matrix());
            renderer_instance_->set_uv(uv_rect_);
            renderer_instance_->set_texture(texture_);
        }
    }
}

void Image::on_unregister_render()
{
    renderer_instance_->destroy();
    renderer_instance_ = nullptr;
}

void Image::matrix_updated(const Matrix4x4& matrix)
{
    if (renderer_instance_) renderer_instance_->set_mvp(matrix);
}

void Image::color_updated(const Color& color)
{
    if (renderer_instance_) renderer_instance_->set_color(color);
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

    if (renderer_instance_) renderer_instance_->set_uv(uv_rect_);
}
