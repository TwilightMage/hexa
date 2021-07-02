#pragma once

#include "UIElement.h"
#include "Engine/Rect.h"
#include "Engine/Slot.h"
#include "Engine/Texture.h"

class RendererUIInstance;

class EXPORT Image : public UIElement, public IRenderable
{
public:
    Image();
    ~Image()
    {
        
    }
    explicit Image(const Shared<Texture>& texture);
    
    Shared<Texture> get_texture() const;
    Quaternion get_uv_rect() const;
    const Rect& get_rect() const;
    void set_rect(const Rect& rect);
    void clear_rect();
    
    void use_shader(const Shared<Shader>& shader);
    void use_texture(const Shared<Texture>& texture);

protected:
    void on_register_render() override;
    void on_unregister_render() override;
    
private:
    void update_uv_rect();

    Shared<RendererUIInstance> renderer_instance_;
    Slot<Texture> texture_;
    Rect rect_;
    bool have_rect_;
    Quaternion uv_rect_;
};
