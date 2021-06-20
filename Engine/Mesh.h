#pragma once

#include "Object.h"
#include "Path.h"
#include "Vector3.h"

class World;
class Entity;

class EXPORT Mesh : public Object
{
    friend Entity;
    friend World;

public:
    struct Vertex
    {
        Vector3 pos;
        Vector2 uv;
        Vector3 col;
    };

    Mesh(const String& name);
    Mesh(const String& name, const List<Vertex>& vertices);
    Mesh(const String& name, const List<Vertex>& vertices, const List<uint>& indices);
    
    static Shared<Mesh> load_obj(const Path& path);

    const List<Vertex>& get_vertices() const;
    const List<uint>& get_indices() const;
    
    uint get_usage_count() const;
    bool is_empty() const;

    static Shared<Mesh> empty;

private:
    List<Vertex> vertices_;
    List<uint> indices_;
    uint usage_count_;
};
