#pragma once

#include <glm/ext/quaternion_float.hpp>

#include "BasicTypes.h"
#include "framework.h"
#include "Matrix4x4.h"

class Mesh;
class Texture;
class Shader;

class EXPORT IRenderable
{
public:
    virtual Shared<Mesh> get_mesh() const = 0;
    virtual Shared<Shader> get_shader() const = 0;
    virtual Shared<Texture> get_texture() const = 0;
    virtual Matrix4x4 get_matrix() const = 0;
};
