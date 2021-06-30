#pragma once

#include <map>

#include "BasicTypes.h"
#include "Matrix4x4.h"

class Texture;
class Shader;
class Mesh;
class IRenderable;
class World;

class EXPORT IRenderer
{
public:
    virtual bool register_object(const Shared<IRenderable>& renderable) const = 0;
    virtual bool unregister_object(const Shared<IRenderable>& renderable) const = 0;
    virtual bool change_object_mesh(const Shared<IRenderable>& renderable, const Shared<Mesh>& old_mesh) = 0;
    virtual bool change_object_shader(const Shared<IRenderable>& renderable, const Shared<Shader>& old_shader) = 0;

    virtual std::map<Texture*, uint> dump_texture_usage() const = 0;
    //virtual std::map<Mesh*, uint> dump_mesh_usage() const = 0;
    //virtual std::map<Shader*, uint> dump_shader_usage() const = 0;
    
    virtual void render(const Matrix4x4& view, const Matrix4x4& projection, const Shared<World>& world) const = 0;
    virtual void cleanup() const = 0;
};
