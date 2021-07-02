#pragma once

#include "BasicTypes.h"

class Material;

class IRenderable
{
public:
    virtual void set_material(const Shared<Material>& material) = 0;
    virtual Shared<Material> get_material() const = 0;
};
