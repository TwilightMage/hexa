#pragma once

#include "UIElement.h"
#include "Engine/IRenderable.h"
#include "Engine/MaterialParameters.h"
#include "Engine/Rect.h"
#include "Engine/Slot.h"
#include "Engine/Texture.h"

class MaterialUI;
class MaterialUIInstance;

class EXPORT Image : public UIElement, public IRenderable<MaterialUI, MaterialUIInstance>
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
    
    void use_texture(const Shared<Texture>& texture);

    void set_material(const Shared<MaterialUI>& material) override;
    FORCEINLINE Shared<MaterialUI> get_material() const override { return material_; }
    FORCEINLINE Shared<MaterialUIInstance> get_material_instance() const override { return material_instance_; }

protected:
    void on_register_render() override;
    void on_unregister_render() override;
    void matrix_updated(const Matrix4x4& matrix) override;
    void color_updated(const Color& color) override;
    
private:
    void update_uv_rect();
    void material_changed();

    Shared<Texture> texture_;
    Rect rect_;
    bool have_rect_;
    Quaternion uv_rect_;

    Shared<MaterialUIInstance> material_instance_;
    Shared<MaterialUI> material_;
    Shared<MaterialParameter<Matrix4x4>> model_parameter_;
};
