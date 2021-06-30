#pragma once

#include "framework.h"
#include "IRenderer.h"
#include "RendererUtils.h"

class Image;

class EXPORT UIRenderer : public IRenderer
{
public:
    UIRenderer();
    ~UIRenderer();
    
    bool register_object(const Shared<IRenderable>& renderable) const override;
    bool unregister_object(const Shared<IRenderable>& renderable) const override;
    bool change_object_mesh(const Shared<IRenderable>& renderable, const Shared<Mesh>& old_mesh) override;
    bool change_object_shader(const Shared<IRenderable>& renderable, const Shared<Shader>& old_shader) override;
    std::map<Texture*, uint> dump_texture_usage() const override;
    void render(const Matrix4x4& view, const Matrix4x4& projection, const Shared<World>& world) const override;
    void cleanup() const override;

private:
    render_database<Image>* database_ = nullptr;
};
