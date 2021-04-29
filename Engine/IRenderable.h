#pragma once

#include "BasicTypes.h"

class IRenderable
{
public:
    virtual void use_mesh(class Mesh* new_mesh) = 0;
    virtual void clear_mesh() = 0;
    virtual Weak<class Mesh> get_mesh() const = 0;
    virtual Weak<class Shader> get_shader() const = 0;
};
