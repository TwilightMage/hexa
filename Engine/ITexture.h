#pragma once

#include "BasicTypes.h"
#include "Path.h"

class EXPORT ITexture
{
public:
    virtual uint get_gl_texture_id() = 0;

    virtual void save_to_file(const Path& path) = 0;
};
