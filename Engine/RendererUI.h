#pragma once

#include "Renderer.h"

class RendererUI : public Renderer
{
public:

protected:
    bool verify_shader_valid(const Shared<Shader>& shader) override;

    Shared<RendererInstance> create_instance_object() override;
};
