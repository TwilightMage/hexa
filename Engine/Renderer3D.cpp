#include "Renderer3D.h"

#include "Renderer3DInstance.h"
#include "RendererInstance.h"

bool Renderer3D::verify_shader_valid(const Shared<Shader>& shader)
{
    return
    shader->check_uniform_presence("model", ShaderDataType::Mat4, true) &&
    shader->check_uniform_presence("texture", ShaderDataType::Usampler2D, true);
}

Shared<RendererInstance> Renderer3D::create_instance_object()
{
    return MakeShared<Renderer3DInstance>();
}
