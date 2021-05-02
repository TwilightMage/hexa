#pragma once
#include <glm/fwd.hpp>


#include "List.h"

class Mesh;
class Shader;
class IRenderable;

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

    void render(const glm::mat4& view_projection_matrix) const;
    void cleanup() const;

private:
    void* database_ = nullptr;
};
