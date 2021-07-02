#pragma once

#include "Engine/Color.h"
#include "Engine/List.h"
#include "Engine/Matrix4x4.h"
#include "Engine/Quaternion.h"
#include "Engine/Vector3.h"

class Game;

class EXPORT UIElement : public std::enable_shared_from_this<UIElement>
{
    friend Game;
    
public:
    UIElement();

    FORCEINLINE const Vector3& get_position() const;
    void set_position(const Vector2& vec2_pos);
    void set_z(float z);
    void set_position(const Vector3& vec3_pos);
    float get_rotation_angle() const;
    void set_rotation(float angle);
    FORCEINLINE Vector2 get_size() const;
    void set_size(const Vector2& vec2_size);
    FORCEINLINE const Color& get_color() const;
    void set_color(const Color& color);
    FORCEINLINE const Matrix4x4& get_ui_matrix() const;

    FORCEINLINE bool get_mouse_detection() const;
    void set_mouse_detection(bool state);
    
    FORCEINLINE Shared<UIElement> get_parent() const;
    const List<Shared<UIElement>>& get_children() const;
    void add_child(const Shared<UIElement>& child);
    void remove_from_parent();
    void remove_all_children();

    FORCEINLINE bool should_render() const;
    FORCEINLINE bool is_started_construction() const;
    FORCEINLINE bool is_constructed() const;
    FORCEINLINE bool is_mouse_over() const;
    FORCEINLINE bool is_pressed() const;
    FORCEINLINE bool is_in_hierarchy() const;

protected:
    virtual void on_register_render();
    virtual void on_unregister_render();
    virtual void on_construct();
    virtual void on_size_changed();
    virtual void on_parent_size_changed();
    virtual void on_mouse_enter();
    virtual void on_mouse_leave();
    virtual void on_press(const Vector2& point);
    virtual void on_release();
    virtual void on_child_added(const Shared<UIElement>& child);
    virtual void on_child_removed(const Shared<UIElement>& child);
    virtual void on_all_child_removed();
    virtual void matrix_updated(const Matrix4x4& matrix);
    virtual void color_updated(const Color& color);
    
    void set_size_internal(const Vector2& vec2_size);
    
private:
    void detect_topmost_under_mouse(Vector2 mouse_pos, float parent_z, Shared<UIElement>& topmost, float& topmost_z);
    bool is_rect_under_mouse(const Vector2& mouse) const;
    void added_to_hierarchy();
    void removed_from_hierarchy();
    void construct();
    void update_matrix();
    void update_matrix_child(const Matrix4x4& parent_matrix);
    void remove_child_internal(const Shared<UIElement>& item);
    
    Vector3 position_;
    Quaternion rotation_;
    Vector3 size_;
    Color color_ = Color::white();
    List<Shared<UIElement>> children_;
    Weak<UIElement> parent_;
    bool mouse_detection_;
    Matrix4x4 trans_rot_matrix_;
    Matrix4x4 trans_rot_matrix_stacked_;
    Matrix4x4 trans_rot_size_matrix_;
    bool should_render_ = false;
    bool is_in_hierarchy_ = false;
    bool is_started_construction_ = false;
    bool constructed_ = false;
    bool is_mouse_over_ = false;
    bool is_pressed_ = false;
};
