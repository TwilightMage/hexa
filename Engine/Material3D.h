#pragma once

#include "Material.h"
#include "Shader.h"

class EXPORT Material3D : public Material
{
public:
    Material3D() : Material(){};
    
protected:
    void register_direct_parameters() override;
    void apply_params(const RenderData& render_data) const override;

    Shared<MaterialInstance> create_instance_object() override;

private:
    Shared<MaterialParameter<Matrix4x4>> view;
    Shared<MaterialParameter<Matrix4x4>> projection;
};
