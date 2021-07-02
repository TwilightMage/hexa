#pragma once
#include "BasicTypes.h"

class Renderer;

class IRenderable
{
public:
    virtual void set_renderer(const Shared<Renderer>& renderer) = 0;
    virtual Shared<Renderer> get_renderer() const = 0;
};
