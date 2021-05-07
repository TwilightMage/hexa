#pragma once

#include "framework.h"
#include "IRenderer.h"
#include "RendererUtils.h"

class Image;

EXTERN class EXPORT UIRenderer : public IRenderer
{
public:
    UIRenderer();
    ~UIRenderer();
    
    bool register_object(const Weak<IRenderable>& renderable) const override;
    bool unregister_object(const Weak<IRenderable>& renderable) const override;
    bool change_object_mesh(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh) override;
    bool change_object_shader(const Weak<IRenderable>& renderable, const Weak<Shader>& old_shader) override;
    std::map<Texture*, uint> dump_texture_usage() const override;
    void render(const glm::mat4& view_projection_matrix) const override;
    void cleanup() const override;

private:
    render_database<Image>* database_ = nullptr;
};
