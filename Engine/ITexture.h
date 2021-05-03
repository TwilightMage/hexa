#pragma once

#include "BasicTypes.h"

class ITexture
{
public:
    virtual uint get_gl_id() = 0;
};
