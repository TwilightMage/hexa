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
    
    void set_position(const Vector2& vec2_pos);
    void set_z(float z);
    void set_position(const Vector3& vec3_pos);
    float get_rotation_angle() const;
    void set_rotation(float angle);
    Vector2 get_size() const;
    void set_size(const Vector2& vec2_size);
    const glm::mat4& get_ui_matrix() const;

    void add_child(const Weak<UIElement>& child);
    void remove_from_parent();

    bool should_render() const;
    bool is_constructed() const;

protected:
    virtual void on_register_render();
    virtual void on_unregister_render();
    virtual void on_construct();
    virtual void on_size_changed();

private:
    void register_render();
    void unregister_render();
    void update_matrix();
    void update_matrix_child(const glm::mat4& parent_matrix);
    
    Vector3 position_;
    Quaternion rotation_;
    Vector3 size_;
    List<Shared<UIElement>> children_;
    Weak<UIElement> parent_;
    glm::mat4 trans_rot_matrix_;
    glm::mat4 trans_rot_matrix_stacked_;
    glm::mat4 trans_rot_size_matrix_;
    bool constructed_ = false;
    bool should_render_ = false;
};
