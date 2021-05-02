#pragma once

#include <glm/ext/quaternion_float.hpp>

#include "BasicTypes.h"
#include "framework.h"
#include "Quaternion.h"
#include "Vector3.h"

EXTERN class EXPORT IRenderable
{
public:
    virtual void use_mesh(const Weak<class Mesh>& new_mesh) = 0;
    virtual void clear_mesh() = 0;
    virtual Weak<class Mesh> get_mesh() const = 0;
    virtual Weak<class Shader> get_shader() const = 0;
    virtual Vector3 get_position() const = 0;
    virtual Quaternion get_rotation() const = 0;
    virtual Vector3 get_scale() const { return Vector3::one(); }
};
