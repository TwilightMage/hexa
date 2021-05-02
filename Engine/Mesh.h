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

    Mesh();
    Mesh(const List<vertex>& vertices);
    Mesh(const List<vertex>& vertices, const List<uint>& indices);
    
    static Shared<Mesh> load_obj(const Path& path);

    const List<vertex>& get_vertices() const;
    const List<uint>& get_indices() const;
    
    uint get_usage_count() const;

//private:
    List<vertex> vertices_;
    List<uint> indices_;
    uint usage_count_;
};
