#pragma once

#include <glm/ext/quaternion_float.hpp>

#include "BasicTypes.h"

class IRenderable
{
public:
    virtual void use_mesh(class Mesh* new_mesh) = 0;
    virtual void clear_mesh() = 0;
    virtual Weak<class Mesh> get_mesh() const = 0;
    virtual Weak<class Shader> get_shader() const = 0;
    virtual glm::vec3 get_position() = 0;
    virtual glm::quat get_rotation() = 0;
    virtual glm::vec3 get_scale() { return glm::vec3(1.0f, 1.0f, 1.0f); }
};
