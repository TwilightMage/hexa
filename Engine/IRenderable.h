#pragma once

class IRenderable
{
public:
    virtual void use_mesh(class Mesh* new_mesh) = 0;
    virtual void clear_mesh() = 0;
    virtual class Mesh* get_mesh() const = 0;
    virtual class Shader* get_shader() const = 0;
};
