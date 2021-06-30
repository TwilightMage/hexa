#pragma once

#include "framework.h"
#include "IRenderer.h"
#include "RendererUtils.h"

class EXPORT Renderer : public IRenderer
{
public:
    Renderer();
    ~Renderer();
    
    bool register_object(const Shared<IRenderable>& renderable) const override;
    bool unregister_object(const Shared<IRenderable>& renderable) const override;
    bool change_object_mesh(const Shared<IRenderable>& renderable, const Shared<Mesh>& old_mesh) override;
    bool change_object_shader(const Shared<IRenderable>& renderable, const Shared<Shader>& old_shader) override;

    std::map<Texture*, uint> dump_texture_usage() const override;
    
    void render(const Matrix4x4& view, const Matrix4x4& projection, const Shared<World>& world) const override;
    void cleanup() const override;

protected:
    virtual void put_params_for_intances(const Matrix4x4& view, const Matrix4x4& projection, const Shared<IRenderable>* objects, uint instance_count, const Shared<World>& world) const;

private:
    render_database<IRenderable>* database_ = nullptr;
};
