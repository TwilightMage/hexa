#pragma once

#include "BasicTypes.h"

class ITexture
{
public:
    virtual uint get_gl_texture_id() = 0;
};
