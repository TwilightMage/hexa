#pragma once

#include "UIElement.h"
#include "Engine/IRenderable.h"
#include "Engine/Rect.h"

EXTERN class EXPORT Image : public UIElement, public IRenderable
{
public:
    Image();
    
    Shared<Mesh> get_mesh() const override;
    Shared<Shader> get_shader() const override;
    Shared<Texture> get_texture() const override;
    Vector3 get_position() const override;
    Quaternion get_rotation() const override;
    Vector3 get_scale() const override;
    Quaternion get_uv_rect() const;
    const Rect& get_rect() const;
    void set_rect(const Rect& rect);
    void clear_rect();
    
    void use_shader(const Weak<Shader>& shader);
    void use_texture(const Weak<Texture>& texture);

protected:
    void on_register_render() override;
    void on_unregister_render() override;
    
private:
    void update_uv_rect();

    Shared<Shader> shader_;
    Shared<Texture> texture_;
    Rect rect_;
    bool have_rect_;
    Quaternion uv_rect_;
};
