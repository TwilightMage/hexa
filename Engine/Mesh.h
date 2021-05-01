#pragma once

#include "Path.h"

EXTERN class EXPORT Mesh
{
    friend class Entity;

public:
    struct vertex
    {
        float pos[3];
        float uv[2];
        float col[3];
    };
    
    static Shared<Mesh> load_obj(const Path& path);

    void optimize();
    
    uint get_usage_count() const;

    List<vertex> vertices; // TODO: Make private
    List<uint> indices;
    uint usage_count_;
};
