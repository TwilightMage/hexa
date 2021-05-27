#pragma once

#include "BasicTypes.h"

EXTERN class EXPORT ITexture
{
public:
    virtual uint get_gl_texture_id() = 0;
};
