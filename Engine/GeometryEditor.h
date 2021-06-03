#pragma once

#include "Color.h"
#include "framework.h"
#include "List.h"
#include "Mesh.h"
#include "Quaternion.h"

EXTERN class EXPORT GeometryEditor
{
public:
    struct Face
    {
        uint size;
        uint index;
    };
    
    static void optimize(List<Mesh::Vertex>& vertices, List<uint>& indices);
    static void optimize_collision(List<Mesh::Vertex>& vertices, List<uint>& indices);
    static void remove_indices(List<Mesh::Vertex>& vertices, List<uint>& indices);
    static void invert_vertices(List<Mesh::Vertex>& vertices);
    static void invert_indices(List<uint>& indices);
    static void randomize_colors(List<Mesh::Vertex>& vertices);
    static void set_color(List<Mesh::Vertex>& vertices, const Color& color);
    static void mirror_x(List<Mesh::Vertex>& vertices);
    static void mirror_y(List<Mesh::Vertex>& vertices);
    static void mirror_z(List<Mesh::Vertex>& vertices);
    static void translate(List<Mesh::Vertex>& vertices, const Vector3& offset);
    static void rotate(List<Mesh::Vertex>& vertices, const Quaternion& quat);
    static void scale(List<Mesh::Vertex>& vertices, const Vector3& factor);
    static void move_to_center(List<Mesh::Vertex>& vertices);
    static void generate_faces(const List<Mesh::Vertex>& vertices, const List<uint>& indices, List<Face>& out_faces, List<uint>& out_indices);

    static Shared<Mesh> get_unit_cube();
};
