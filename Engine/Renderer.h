#pragma once
#include <map>
#include <glm/fwd.hpp>


#include "List.h"

class Texture;
class Mesh;
class Shader;
class IRenderable;

struct render_database;

/* STRUCTURE EXAMPLE
*
* Game
* |
* +--basic_shader
* |  |
* |  +--player_model
* |  |  |
* |  |  +--player_1
* |  |  |
* |  |  +--player_2
* |  |
* |  +--wolf_model
* |     |
* |     +--wolf_1
* |     |
* |     +--wolf_2
* |     |
* |     +--wolf_3
* |
* +--transparent_shader
*    |
*    +--bottle_model
*       |
*       +--bottle_1
*/

EXTERN class EXPORT Renderer
{
public:
    Renderer();
    ~Renderer();
    
    void register_object(const Weak<IRenderable>& renderable) const;
    void unregister_object(const Weak<IRenderable>& renderable) const;
    void change_object_mesh(const Shared<IRenderable>& renderable, const Weak<Mesh>& old_mesh);
    void change_object_shader(const Shared<IRenderable>& renderable, const Weak<Shader>& old_shader);

    std::map<Texture*, uint> dump_texture_usage() const;
    
    void render(const glm::mat4& view_projection_matrix, class TextureAtlas* atlas) const;
    void cleanup() const;

private:
    render_database* database_ = nullptr;
};
