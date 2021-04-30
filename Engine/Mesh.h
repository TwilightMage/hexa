#pragma once
#include "linmath.h"
#include "Path.h"

class Mesh
{
    friend class Entity;

public:
    struct vertex
    {
        vec3 pos;
        vec2 uv;
        vec3 col;
    };
    
    static Shared<Mesh> load_obj(const Path& path);

    uint get_usage_count() const;

    List<vertex> vertices; // TODO: Make private
    List<uint> indices;
    uint usage_count_;
};
