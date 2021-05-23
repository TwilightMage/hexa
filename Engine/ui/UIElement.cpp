﻿#include "UIElement.h"

#include <glm/detail/type_mat4x4.hpp>

#include "Engine/Game.h"
#include "Engine/Texture.h"
#include "Engine/Utils.h"

UIElement::UIElement()
    : size_(Vector3::one())
    , mouse_detection_(true)
{
}

const Vector3& UIElement::get_position() const
{
    return position_;
}

void UIElement::set_position(const Vector2& vec2_pos)
{
    if (position_.x == vec2_pos.x && position_.y == vec2_pos.y) return;
    
    position_.x = vec2_pos.x;
    position_.y = vec2_pos.y;

    update_matrix();
}

void UIElement::set_z(float z)
{
    if (position_.z == z) return;
    
    position_.z = z;

    update_matrix();
}

void UIElement::set_position(const Vector3& vec3_pos)
{
    if (position_ == vec3_pos) return;
    
    position_ = vec3_pos;

    update_matrix();
}

float UIElement::get_rotation_angle() const
{
    return 0.0f; // TODO: add rotation angle to UI
}

void UIElement::set_rotation(float angle)
{
    const auto rot = Quaternion(Vector3(angle, 0.0f, 0.0f));

    if (rotation_ == rot) return;
    
    rotation_ = rot;

    update_matrix();
}

Vector2 UIElement::get_size() const
{
    return size_;
}

void UIElement::set_size(const Vector2& vec2_size)
{
    if (size_ == vec2_size) return;
    
    set_size_internal(vec2_size);

    on_size_changed();
    for (auto& child : children_)
    {
        child->on_parent_size_changed();
    }
}

const Matrix4x4& UIElement::get_ui_matrix() const
{
    return trans_rot_size_matrix_;
}

bool UIElement::get_mouse_detection() const
{
    return mouse_detection_;
}

void UIElement::set_mouse_detection(bool state)
{
    mouse_detection_ = state;
}

Weak<UIElement> UIElement::get_parent() const
{
    return parent_;
}

const List<Shared<UIElement>>& UIElement::get_children() const
{
    return children_;
}

void UIElement::add_child(const Weak<UIElement>& child)
{
    if (const auto child_ptr = child.lock())
    {       
        if (const auto old_parent = child_ptr->parent_.lock())
        {
            old_parent->children_.Remove(child_ptr);
            old_parent->on_child_removed(child_ptr);
        }
        else
        {
            if (auto child_renderable = cast<IRenderable>(child_ptr))
            {
                if (Game::instance_->ui_renderer_->register_object(child_renderable))
                {
                    child_ptr->register_render();
                }
            }
        }
        
        child_ptr->parent_ = weak_from_this();
        children_.Add(child_ptr);
        on_child_added(child_ptr);

        child_ptr->update_matrix();

        child_ptr->construct();
    }
}

void UIElement::remove_from_parent()
{
    if (const auto parent_ptr = parent_.lock())
    {
        parent_ptr->children_.Remove(shared_from_this());
        parent_ptr->on_child_removed(shared_from_this());

        if (auto me_renderable = cast<IRenderable>(shared_from_this()))
        {
            if (Game::instance_->ui_renderer_->unregister_object(me_renderable))
            {
                unregister_render();
            }
        }
    }
}

void UIElement::remove_all_children()
{
    while (children_.length() > 0)
    {
        children_.last()->remove_from_parent();
    }

    on_all_child_removed();
}

bool UIElement::should_render() const
{
    return should_render_;
}

bool UIElement::is_started_construction() const
{
    return is_started_construction_;
}

bool UIElement::is_constructed() const
{
    return constructed_;
}

bool UIElement::is_mouse_over() const
{
    return is_mouse_over_;
}

bool UIElement::is_pressed() const
{
    return is_pressed_;
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

void UIElement::construct()
{
    if (!constructed_)
    {
        is_started_construction_ = true;
        on_construct();
        constructed_ = true;
    }

    for (auto& child : children_)
    {
        child->construct();
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

void UIElement::on_parent_size_changed()
{
}

void UIElement::on_mouse_enter()
{
}

void UIElement::on_mouse_leave()
{
}

void UIElement::on_press()
{
}

void UIElement::on_release()
{
}

void UIElement::on_child_added(const Shared<UIElement>& child)
{
}

void UIElement::on_child_removed(const Shared<UIElement>& child)
{
}

void UIElement::on_all_child_removed()
{
}

void UIElement::set_size_internal(const Vector2& vec2_size)
{
    size_ = vec2_size;

    update_matrix();
}

void UIElement::detect_topmost_under_mouse(Vector2 mouse_pos, float parent_z, Shared<UIElement>& topmost, float& topmost_z)
{
    mouse_pos -= position_ * Game::get_ui_scale();
    if (is_rect_under_mouse(mouse_pos / Game::get_ui_scale()))
    {
        const auto my_z = parent_z + position_.z + 0.001f;
        
        if (mouse_detection_ && my_z >= topmost_z)
        {
            topmost = shared_from_this();
            topmost_z = my_z;
        }
        
        for (const auto& child : children_)
        {
            child->detect_topmost_under_mouse(mouse_pos, my_z, topmost, topmost_z);
        }
    }
}

bool UIElement::is_rect_under_mouse(const Vector2& mouse) const
{
    return mouse.x >= 0 && mouse.x < size_.x && mouse.y >= 0 && mouse.y < size_.y;
}

void UIElement::update_matrix()
{
    trans_rot_matrix_ = Matrix4x4();

    Vector3 trans = position_;
    trans.z += 0.001f;
    trans.y *= -1;
    trans_rot_matrix_ = Matrix4x4().translate(trans * Game::get_ui_scale()).rotate(rotation_);
    
    Matrix4x4 parent_matrix;
    if (const auto& parent = parent_.lock())
    {
        parent_matrix = parent->trans_rot_matrix_stacked_;
    }
    update_matrix_child(parent_matrix);
}

void UIElement::update_matrix_child(const Matrix4x4& parent_matrix)
{
    trans_rot_matrix_stacked_ = parent_matrix * trans_rot_matrix_;

    uint i = 0;
    for (auto& child : children_)
    {
        child->update_matrix_child(trans_rot_matrix_stacked_);
        i++;
    }

    trans_rot_size_matrix_ = trans_rot_matrix_stacked_.scale(size_ * Game::get_ui_scale());
}
