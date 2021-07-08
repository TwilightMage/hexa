#pragma once

#include "BasicTypes.h"

class Material;
class MaterialInstance;

template<class MaterialClass, class MaterialInstanceClass>
class IRenderable
{
public:
    virtual void set_material(const Shared<MaterialClass>& material) = 0;
    virtual Shared<MaterialClass> get_material() const = 0;
    virtual Shared<MaterialInstanceClass> get_material_instance() const = 0;
};
