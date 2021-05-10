#pragma once

#include <glm/detail/type_mat4x4.hpp>

#include "Engine/List.h"
#include "Engine/Quaternion.h"
#include "Engine/Vector3.h"

class Game;

EXTERN class EXPORT UIElement : public std::enable_shared_from_this<UIElement>
{
    friend Game;
    
public:
    UIElement();

    const Vector3& get_position() const;
    void set_position(const Vector2& vec2_pos);
    void set_z(float z);
    void set_position(const Vector3& vec3_pos);
    float get_rotation_angle() const;
    void set_rotation(float angle);
    Vector2 get_size() const;
    void set_size(const Vector2& vec2_size);
    const glm::mat4& get_ui_matrix() const;

    bool get_mouse_detection() const;
    void set_mouse_detection(bool state);
    
    Weak<UIElement> get_parent() const;
    void add_child(const Weak<UIElement>& child);
    void remove_from_parent();
    void remove_all_children();

    bool should_render() const;
    bool is_started_construction() const;
    bool is_constructed() const;
    bool is_mouse_over() const;
    bool is_pressed() const;

protected:
    virtual void on_register_render();
    virtual void on_unregister_render();
    virtual void on_construct();
    virtual void on_size_changed();
    virtual void on_parent_size_changed();
    virtual void on_mouse_enter();
    virtual void on_mouse_leave();
    virtual void on_press();
    virtual void on_release();
    
private:
    void detect_topmost_under_mouse(Vector2 mouse_pos, float parent_z, Shared<UIElement>& topmost, float& topmost_z);
    bool is_rect_under_mouse(const Vector2& mouse) const;
    void register_render();
    void unregister_render();
    void construct();
    void update_matrix();
    void update_matrix_child(const glm::mat4& parent_matrix);
    
    Vector3 position_;
    Quaternion rotation_;
    Vector3 size_;
    List<Shared<UIElement>> children_;
    Weak<UIElement> parent_;
    bool mouse_detection_;
    glm::mat4 trans_rot_matrix_;
    glm::mat4 trans_rot_matrix_stacked_;
    glm::mat4 trans_rot_size_matrix_;
    bool should_render_ = false;
    bool is_started_construction_ = false;
    bool constructed_ = false;
    bool is_mouse_over_ = false;
    bool is_pressed_ = false;
};
