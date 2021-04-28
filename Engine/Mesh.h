#pragma once
#include "linmath.h"
#include "Path.h"

class Mesh
{
public:
    struct Vertex
    {
        vec3 pos;
        vec2 uv;
        vec3 col;
    };
    
    static Mesh* load_obj(const Path& path);

private:
    List<Vertex> vertices;
};
