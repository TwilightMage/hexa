#pragma once

#include <glm/ext/quaternion_float.hpp>

#include "BasicTypes.h"
#include "framework.h"
#include "Quaternion.h"
#include "Vector3.h"

class Mesh;
class Texture;
class Shader;

EXTERN class EXPORT IRenderable
{
public:
    virtual Shared<Mesh> get_mesh() const = 0;
    virtual Shared<Shader> get_shader() const = 0;
    virtual Shared<Texture> get_texture() const = 0;
    virtual Vector3 get_position() const = 0;
    virtual Quaternion get_rotation() const = 0;
    virtual Vector3 get_scale() const { return Vector3::one(); }
};
