#pragma once
#include <map>
#include <glm/fwd.hpp>


#include "BasicTypes.h"

class Texture;
class Shader;
class Mesh;
class IRenderable;

class IRenderer
{
public:
    virtual void register_object(const Weak<IRenderable>& renderable) const = 0;
    virtual void unregister_object(const Weak<IRenderable>& renderable) const = 0;
    virtual void change_object_mesh(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh) = 0;
    virtual void change_object_shader(const Weak<IRenderable>& renderable, const Weak<Shader>& old_shader) = 0;

    virtual std::map<Texture*, uint> dump_texture_usage() const = 0;
    //virtual std::map<Mesh*, uint> dump_mesh_usage() const = 0;
    //virtual std::map<Shader*, uint> dump_shader_usage() const = 0;
    
    virtual void render(const glm::mat4& view_projection_matrix) const = 0;
    virtual void cleanup() const = 0;
};
