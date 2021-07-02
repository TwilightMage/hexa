#pragma once

#include "Material.h"

class EXPORT MaterialUI : public Material
{
protected:
    bool verify_shader_valid(const Shared<Shader>& shader) override;

    void register_direct_parameters() override;
    void apply_params(const RenderData& render_data) const override;

    Shared<MaterialInstance> create_instance_object() override;

private:
    Shared<MaterialParameter<Matrix4x4>> projection;
};
