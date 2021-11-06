#pragma once

#include "Object.h"
#include "Path.h"
#include "Pointers.h"
#include "Quaternion.h"
#include "Vector3.h"

class MeshComponent;
class Collision;
class World;
class Entity;

namespace Ogre
{
    class SubMesh;
    class Mesh;
}

enum class AutoCollisionMode
{
    None,
    Default,
    Convex,
    Complex
};

class EXPORT StaticMesh : public Object
{
    friend Entity;
    friend World;
    friend MeshComponent;

public:
    struct Vertex
    {
        Vector3 pos;
        Vector2 uv;
        Vector3 norm;
    };

    struct Triangle
    {
        uint i0;
        uint i1;
        uint i2;
    };

    struct EXPORT SubMesh
    {
        SubMesh(const String& name, const List<Vertex>& vertices, const List<uint>& indices);
        SubMesh(const List<Vertex>& vertices, const List<uint>& indices);
        SubMesh();

        void add(const List<Vertex>& new_vertices, const List<uint>& new_indices);
        
        String name;
        List<Vertex> vertices;
        List<uint> indices;
    };

    struct CollisionShapeInfo
    {
        Vector3 location;
        Quaternion rotation;
        Shared<Collision> collision;
    };

    explicit StaticMesh(const String& name);
    
    static Shared<StaticMesh> construct(const String& name, const List<SubMesh>& sub_meshes, AutoCollisionMode collision_mode = AutoCollisionMode::Default);
    static Shared<StaticMesh> load_file_obj(const Path& path, AutoCollisionMode collision_mode = AutoCollisionMode::Default);

    uint get_material_count() const;

    FORCEINLINE const Vector3& get_bounds_center() const;
    FORCEINLINE const Vector3& get_bounds_half_size() const;
    bool is_empty() const;

    void make_instanced();

    static Shared<StaticMesh> empty;

private:
    static Shared<StaticMesh> create(const String& name, const List<SubMesh>& sub_meshes, AutoCollisionMode collision_mode, bool compute_normals);

    List<CollisionShapeInfo> collisions_;
    
    Shared<Ogre::Mesh> ogre_mesh_;

    bool instanced_;
};
