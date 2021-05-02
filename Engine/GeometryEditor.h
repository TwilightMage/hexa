#pragma once

#include "framework.h"
#include "List.h"
#include "Mesh.h"
#include "Quaternion.h"

EXTERN class EXPORT GeometryEditor
{
public:
    static void optimize(List<Mesh::vertex>& vertices, List<uint>& indices);
    static void remove_indices(List<Mesh::vertex>& vertices, List<uint>& indices);
    static void invert_vertices(List<Mesh::vertex>& vertices);
    static void invert_indices(List<uint>& indices);
    static void randomize_colors(List<Mesh::vertex>& vertices);
    static void set_color(List<Mesh::vertex>& vertices, const Vector3& color);
    static void mirror_x(List<Mesh::vertex>& vertices);
    static void mirror_y(List<Mesh::vertex>& vertices);
    static void mirror_z(List<Mesh::vertex>& vertices);
    static void translate(List<Mesh::vertex>& vertices, const Vector3& offset);
    static void rotate(List<Mesh::vertex>& vertices, const Quaternion& quat);
    static void scale(List<Mesh::vertex>& vertices, const Vector3& factor);
};
