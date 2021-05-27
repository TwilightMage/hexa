#pragma once

#include "BasicTypes.h"
#include "Vector2.h"

EXTERN class EXPORT ITextureBinding
{
public:
    virtual uint get_gl_id() = 0;
    virtual Vector2 modify_uv(const Vector2& uv) = 0;
};
