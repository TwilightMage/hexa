#include "Image.h"

#include "Engine/Assert.h"
#include "Engine/Game.h"
#include "Engine/MaterialUI.h"
#include "Engine/MaterialUIInstance.h"
#include "Engine/Texture.h"

Image::Image()
    : UIElement()
    , texture_(Game::get_white_pixel())
    , have_rect_(false)
    , uv_rect_(Quaternion(0.0f, 0.0f, 1.0f, 1.0f))
    , material_(Game::get_basic_material_ui())
{
    
}

Image::Image(const Shared<Texture>& texture)
    : UIElement()
    , texture_(texture)
    , have_rect_(false)
    , uv_rect_(Quaternion(0.0f, 0.0f, 1.0f, 1.0f))
    , material_(cast<MaterialUI>(Game::get_basic_material_ui()))
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

    if (material_instance_) material_instance_->set_param_value("texture", texture_);
}

void Image::set_material(const Shared<MaterialUI>& material)
{
    if (!Check(material_ != nullptr, "UI", "Cannot set material to nullptr")) return;

    material_ = material;

    material_changed();
}

void Image::on_register_render()
{
    material_changed();
    
    material_instance_->set_param_value("color", get_color().to_quaternion());
    material_instance_->set_param_value("uv", uv_rect_);
    material_instance_->set_param_value("texture", texture_);

    if (model_parameter_) model_parameter_->value = get_ui_matrix();
}

void Image::on_unregister_render()
{
    material_instance_->destroy();
    material_instance_ = nullptr;
}

void Image::matrix_updated(const Matrix4x4& matrix)
{
    if (model_parameter_) model_parameter_->value = matrix;
}

void Image::color_updated(const Color& color)
{
    if (material_instance_) material_instance_->set_param_value("color", color);
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

    if (material_instance_) material_instance_->set_param_value("uv", uv_rect_);
}

void Image::material_changed()
{
    auto new_material_instance = cast<MaterialUIInstance>(material_->create_instance());
    
    if (material_instance_)
    {
        new_material_instance->copy_parameters_from(material_instance_);
        material_instance_->destroy();
    }

    model_parameter_ = new_material_instance->get_parameter<Matrix4x4>("model");

    material_instance_ = new_material_instance;
}
