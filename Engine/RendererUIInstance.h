#pragma once

#include "RendererInstance.h"

class EXPORT RendererUIInstance : public RendererInstance
{
public:
    void set_mvp(const Matrix4x4& value) const;
    void set_uv(const Vector2& translate, const Vector2& scale) const;
    void set_uv(const Quaternion& uv_transform) const;
    void set_color(const Color& value) const;
    void set_texture(const Shared<Texture>& value) const;

protected:
    void register_direct_parameters() override;

private:
    Shared<RendererParameter<Matrix4x4>> model;
    Shared<RendererParameter<Quaternion>> uv;
    Shared<RendererParameter<Quaternion>> color;
    Shared<RendererParameter<Shared<Texture>>> texture;
};
