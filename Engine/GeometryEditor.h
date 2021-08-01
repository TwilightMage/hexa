#pragma once

#include "Color.h"
#include "framework.h"
#include "List.h"
#include "StaticMesh.h"
#include "Quaternion.h"

class EXPORT GeometryEditor
{
public:
    struct Face
    {
        uint size;
        uint index;
    };

    FORCEINLINE static Vector3 compute_normal(const Vector3& a, const Vector3& b, const Vector3& c);
    
    static void optimize(List<StaticMesh::Vertex>& vertices, List<uint>& indices);
    static void optimize_collision(List<StaticMesh::Vertex>& vertices, List<uint>& indices);
    static void remove_indices(List<StaticMesh::Vertex>& vertices, List<uint>& indices);
    static void invert_vertices(List<StaticMesh::Vertex>& vertices);
    static void invert_indices(List<uint>& indices);
    static void mirror_x(List<StaticMesh::Vertex>& vertices);
    static void mirror_y(List<StaticMesh::Vertex>& vertices);
    static void mirror_z(List<StaticMesh::Vertex>& vertices);
    static void translate(List<StaticMesh::Vertex>& vertices, const Vector3& offset);
    static void rotate(List<StaticMesh::Vertex>& vertices, const Quaternion& quat);
    static void scale(List<StaticMesh::Vertex>& vertices, const Vector3& factor);
    static void move_to_center(List<StaticMesh::Vertex>& vertices);
    static void compute_faces(const List<StaticMesh::Vertex>& vertices, const List<uint>& indices, List<Face>& out_faces, List<uint>& out_indices);
    static void compute_normals(const List<StaticMesh::Vertex>& vertices, const List<uint>& indices, List<Vector3>& out_normals, bool invert = false);
    static void compute_normals(List<StaticMesh::Vertex>& vertices, const List<uint>& indices, bool invert = false);
    
    static Shared<StaticMesh> get_unit_cube();
};
