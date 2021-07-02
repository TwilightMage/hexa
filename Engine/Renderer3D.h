#pragma once

#include "Renderer.h"
#include "Shader.h"

class EXPORT Renderer3D : public Renderer
{
protected:
    bool verify_shader_valid(const Shared<Shader>& shader) override;

    void register_direct_parameters() override;
    void apply_params(const RenderData& render_data) const override;

    Shared<RendererInstance> create_instance_object() override;

private:
    Shared<RendererParameter<Matrix4x4>> view;
    Shared<RendererParameter<Matrix4x4>> projection;
};
