#pragma once

#include "Path.h"
#include "Vector3.h"

EXTERN class EXPORT Mesh
{
    friend class Entity;

public:
    struct vertex
    {
        Vector3 pos;
        Vector2 uv;
        Vector3 col;
    };
    
    static Shared<Mesh> load_obj(const Path& path);

    void optimize();
    void set_color(const Vector3& rhs);
    void invert();
    Shared<Mesh> remove_indices() const;
    
    uint get_usage_count() const;

    List<vertex> vertices; // TODO: Make private
    List<uint> indices;
    uint usage_count_;
};
