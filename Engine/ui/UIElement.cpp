﻿#include "UIElement.h"

#include "Engine/Texture.h"

UIElement::UIElement()
    : scale_(Vector3::one())
    , trans_rot_matrix_(glm::mat4(1.0f))
    , trans_rot_matrix_stacked_(glm::mat4(1.0f))
    , trans_rot_size_matrix_(glm::mat4(1.0f))
{
}

Vector3 UIElement::get_render_position() const
{
    return position_;
}

void UIElement::set_position(const Vector2& vec2pos)
{
    position_.x = vec2pos.x;
    position_.y = -vec2pos.y;

    update_matrix();
}

void UIElement::set_z(float z)
{
    position_.z = z;

    update_matrix();
}

void UIElement::set_position(const Vector3& vec3pos)
{
    position_.x = vec3pos.x;
    position_.y = -vec3pos.y;
    position_.z = vec3pos.z;

    update_matrix();
}

Quaternion UIElement::get_render_rotation() const
{
    return rotation_;
}

float UIElement::get_rotation_angle() const
{
    return 0.0f; // TODO: add rotation angle to UI
}

void UIElement::set_rotation(float angle)
{
    rotation_ = Quaternion(Vector3(angle, 0.0f, 0.0f));

    update_matrix();
}

Vector3 UIElement::get_render_scale() const
{
    return scale_;
}

Vector2 UIElement::get_size() const
{
    return scale_;
}

void UIElement::set_size(const Vector2& vec2size)
{
    scale_ = vec2size;

    update_matrix();

    on_size_changed();
}

void UIElement::add_child(const Weak<UIElement>& child)
{
    if (const auto child_ptr = child.lock())
    {       
        if (const auto old_parent = child_ptr->parent_.lock())
        {
            old_parent->children_.Remove(child_ptr);
        }
        else
        {
            child_ptr->register_render();
        }
        
        child_ptr->parent_ = weak_from_this();
        children_.Add(child_ptr);

        if (!child_ptr->constructed_)
        {
            child_ptr->on_construct();
            child_ptr->constructed_ = true;
        }
    }
}

void UIElement::remove_from_parent()
{
    if (const auto parent_ptr = parent_.lock())
    {
        parent_ptr->children_.Remove(shared_from_this());

        unregister_render();
    }
}

const glm::mat4& UIElement::get_matrix() const
{
    return trans_rot_size_matrix_;
}

bool UIElement::should_render() const
{
    return should_render_;
}

void UIElement::register_render()
{
    should_render_ = true;
    on_register_render();
    for (auto& child : children_)
    {
        child->register_render();
    }
}

void UIElement::unregister_render()
{
    should_render_ = false;
    on_unregister_render();
    for (auto& child : children_)
    {
        child->unregister_render();
    }
}

void UIElement::on_register_render()
{
}

void UIElement::on_unregister_render()
{
}

void UIElement::on_construct()
{
}

void UIElement::on_size_changed()
{
}

void UIElement::update_matrix()
{
    trans_rot_matrix_ = glm::mat4(1.0f);
    
    trans_rot_matrix_ = translate(trans_rot_matrix_, cast_object<glm::vec3>(get_render_position()));

    trans_rot_matrix_ = rotate(trans_rot_matrix_, get_render_rotation().axis_angle(), cast_object<glm::vec3>(get_render_rotation().axis()));

    glm::mat4 parent_matrix(1.0f);
    if (const auto& parent = parent_.lock())
    {
        parent_matrix = parent->trans_rot_matrix_stacked_;
    }
    update_matrix_child(parent_matrix);
}

void UIElement::update_matrix_child(const glm::mat4& parent_matrix)
{
    trans_rot_matrix_stacked_ = parent_matrix * trans_rot_matrix_;

    for (auto& child : children_)
    {
        child->update_matrix_child(trans_rot_matrix_stacked_);
    }

    trans_rot_size_matrix_ = scale(trans_rot_matrix_stacked_, cast_object<glm::vec3>(get_render_scale()));
}