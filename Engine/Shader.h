#pragma once

#include "Path.h"

class Shader
{
public:
    static Shader* compile(const Path& frag, const Path& vert);
    void link() const;

public:
    uint program;
};
