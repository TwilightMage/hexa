#pragma once

#include "MaterialInstance.h"

class EXPORT MaterialUIInstance : public MaterialInstance
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
    Shared<MaterialParameter<Matrix4x4>> model;
    Shared<MaterialParameter<Quaternion>> uv;
    Shared<MaterialParameter<Quaternion>> color;
    Shared<MaterialParameter<Shared<Texture>>> texture;
};
