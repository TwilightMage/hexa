#include "RendererUI.h"

#include "RendererUIInstance.h"

bool RendererUI::verify_shader_valid(const Shared<Shader>& shader)
{
    return
    shader->check_uniform_presence("mvp", ShaderDataType::Mat4, true) &&
    shader->check_uniform_presence("uv", ShaderDataType::Vec4, true) &&
    shader->check_uniform_presence("color", ShaderDataType::Vec4, true) &&
    shader->check_uniform_presence("texture", ShaderDataType::Usampler2D, true);
}

Shared<RendererInstance> RendererUI::create_instance_object()
{
    return MakeShared<RendererUIInstance>();
}
