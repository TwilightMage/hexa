#pragma once

#include <map>

#include "BasicTypes.h"
#include "Matrix4x4.h"

class Texture;
class Shader;
class Mesh;
class IRenderable;

EXTERN class EXPORT IRenderer
{
public:
    virtual bool register_object(const Weak<IRenderable>& renderable) const = 0;
    virtual bool unregister_object(const Weak<IRenderable>& renderable) const = 0;
    virtual bool change_object_mesh(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh) = 0;
    virtual bool change_object_shader(const Weak<IRenderable>& renderable, const Weak<Shader>& old_shader) = 0;

    virtual std::map<Texture*, uint> dump_texture_usage() const = 0;
    //virtual std::map<Mesh*, uint> dump_mesh_usage() const = 0;
    //virtual std::map<Shader*, uint> dump_shader_usage() const = 0;
    
    virtual void render(const Matrix4x4& view_projection_matrix) const = 0;
    virtual void cleanup() const = 0;
};
