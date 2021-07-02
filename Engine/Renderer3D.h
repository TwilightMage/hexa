#pragma once

#include "Renderer.h"

class Renderer3D : public Renderer
{
public:
    inline static const Shader::Meta default_meta = {
        1,
        false
    };

protected:
    bool verify_shader_valid(const Shared<Shader>& shader) override;

    Shared<RendererInstance> create_instance_object() override;
};
