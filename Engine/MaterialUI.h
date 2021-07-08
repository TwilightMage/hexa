#pragma once

#include "Material.h"

class EXPORT MaterialUI : public Material
{
public:
    MaterialUI() : Material(){};
    
protected:
    void register_direct_parameters() override;
    void apply_params(const RenderData& render_data) const override;

    Shared<MaterialInstance> create_instance_object() override;

private:
    Shared<MaterialParameter<Matrix4x4>> projection;
};
