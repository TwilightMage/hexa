#include "GeometryEditor.h"

#include <map>

#include "Math.h"

Vector3 GeometryEditor::compute_normal(const Vector3& a, const Vector3& b, const Vector3& c)
{
    return (c - a).cross_product(b - a).normalized();
}

void GeometryEditor::optimize(List<Mesh::Vertex>& vertices, List<uint>& indices)
{
    for (uint i = 0; i < vertices.length() - 1; i++)
    {
        for (uint j = i + 1; j < vertices.length(); j++)
        {
            if (memcmp(&vertices[i], &vertices[j], sizeof(Mesh::Vertex)) == 0)
            {
                for (uint k = 0; k < indices.length(); k++)
                {
                    if (indices[k] == j)
                    {
                        indices[k] = i;
                    }
                }

                vertices.RemoveAt(j--);
            }
        }
    }
}

void GeometryEditor::optimize_collision(List<Mesh::Vertex>& vertices, List<uint>& indices)
{
    for (uint i = 0; i < vertices.length() - 1; i++)
    {
        for (uint j = i + 1; j < vertices.length(); j++)
        {
            if (vertices[i].pos == vertices[j].pos)
            {
                for (uint k = 0; k < indices.length(); k++)
                {
                    if (indices[k] == j)
                    {
                        indices[k] = i;
                    }
                    else if (indices[k] > j)
                    {
                        indices[k]--;
                    }
                }

                vertices.RemoveAt(j--);
            }
        }
    }
}

void GeometryEditor::remove_indices(List<Mesh::Vertex>& vertices, List<uint>& indices)
{
    /*for (uint i = 0; i < indices.length() - 1; i++)
    {
        if (indices[i] != i)
        {
            vertices.Insert(vertices[indices[i]], i);
            indices[i] = i;
            for (uint j = i + 1; j < indices.length(); j++)
            {
                if (indices[j] >= i) indices[j]++;
            }
        }
    }*/

    List<Mesh::Vertex> result;
    for (auto index : indices)
    {
        result.Add(vertices[index]);
    }

    vertices = result;
}

void GeometryEditor::invert_vertices(List<Mesh::Vertex>& vertices)
{
    for (uint i = 0; i < vertices.length(); i += 3)
    {
        std::swap(vertices[i], vertices[i + 2]);
    }
}

void GeometryEditor::invert_indices(List<uint>& indices)
{
    for (uint i = 0; i < indices.length(); i += 3)
    {
        std::swap(indices[i], indices[i + 2]);
    }
}

void GeometryEditor::randomize_colors(List<Mesh::Vertex>& vertices)
{
}

void GeometryEditor::set_color(List<Mesh::Vertex>& vertices, const Color& color)
{
    const auto vec_color = color.to_vector3();
    for (auto& vertex : vertices)
    {
        vertex.col = vec_color;
    }
}

void GeometryEditor::mirror_x(List<Mesh::Vertex>& vertices)
{
    for (auto& vertex : vertices)
    {
        vertex.pos.x *= -1;
    }
}

void GeometryEditor::mirror_y(List<Mesh::Vertex>& vertices)
{
    for (auto& vertex : vertices)
    {
        vertex.pos.y *= -1;
    }
}

void GeometryEditor::mirror_z(List<Mesh::Vertex>& vertices)
{
    for (auto& vertex : vertices)
    {
        vertex.pos.z *= -1;
    }
}

void GeometryEditor::translate(List<Mesh::Vertex>& vertices, const Vector3& offset)
{
    for (auto& vertex : vertices)
    {
        vertex.pos += offset;
    }
}

void GeometryEditor::rotate(List<Mesh::Vertex>& vertices, const Quaternion& quat)
{
    for (auto& vertex : vertices)
    {
        vertex.pos = quat.rotate_vector3(vertex.pos);
    }
}

void GeometryEditor::scale(List<Mesh::Vertex>& vertices, const Vector3& factor)
{
    for (auto& vertex : vertices)
    {
        vertex.pos *= factor;
    }
}

void GeometryEditor::move_to_center(List<Mesh::Vertex>& vertices)
{
    Vector3 sum;
    for (const auto& vertex : vertices)
    {
        sum += vertex.pos;
    }
    sum /= static_cast<float>(vertices.length());
    translate(vertices, sum);
}

struct Triangle
{
    uint points[3];
    Vector3 normal;

    bool is_near(const Triangle& triangle)
    {
        byte counter = 0;
        for (byte i = 0; i < 3; i++)
        {
            for (byte j = 0; j < 3; j++)
            {
                if (points[i] == triangle.points[j])
                {
                    ++counter;
                    break;
                }
            }
        }
        return counter > 1;
    }
};

FORCEINLINE float angle(const Vector3& point, const Vector3& front, const Vector3& left)
{
    float angle = Math::acos_deg(Math::clamp(point.dot_product(front), -1.0f, 1.0f));
    float dot = Math::clamp(point.dot_product(left), -1.0f, 1.0f);
    if (dot > 0) angle *= -1;
    return Math::unwind_angle(angle);
}

void GeometryEditor::compute_faces(const List<Mesh::Vertex>& vertices, const List<uint>& indices, List<Face>& out_faces, List<uint>& out_indices)
{
    out_faces.Clear();
    out_indices.Clear();

    List<List<uint>> faces;
    List<Triangle> triangles(indices.length() / 3);
    for (uint i = 0; i < indices.length() / 3; i++)
    {
        triangles[i].points[0] = indices[i * 3 + 0];
        triangles[i].points[1] = indices[i * 3 + 1];
        triangles[i].points[2] = indices[i * 3 + 2];
        triangles[i].normal = compute_normal(vertices[indices[i * 3 + 0]].pos, vertices[indices[i * 3 + 1]].pos, vertices[indices[i * 3 + 2]].pos);
    }

    for (uint i = 0; i < triangles.length(); i++)
    {
        bool face_found = false;
        for (auto& face : faces)
        {
            for (auto face_triangle_index : face)
            {
                if (triangles[i].normal == triangles[face_triangle_index].normal && triangles[i].is_near(triangles[face_triangle_index]))
                {
                    face_found = true;
                    face.Add(i);
                    break;
                }
            }
            
            if (face_found) break;
        }

        if (!face_found)
        {
            faces.Add({ i });
        }
    }

    for (auto& face : faces)
    {
        List<uint> face_vertex_indices;
        for (auto triangle_id : face)
        {
            face_vertex_indices.add_unique(triangles[triangle_id].points[0]);
            face_vertex_indices.add_unique(triangles[triangle_id].points[1]);
            face_vertex_indices.add_unique(triangles[triangle_id].points[2]);
        }
        
        Vector3 center;
        for (auto face_index : face_vertex_indices)
        {
            center += vertices[face_index].pos;
        }
        center /= face_vertex_indices.length();
        Vector3 orient_axis_f = (vertices[face_vertex_indices[0]].pos - center).normalized();
        Vector3 orient_axis_l = orient_axis_f.cross_product(triangles[face[0]].normal);
        std::map<uint, float> angles;
        for (uint i = 0; i < face_vertex_indices.length(); i++)
        {
            angles[face_vertex_indices[i]] = angle((vertices[face_vertex_indices[i]].pos - center).normalized(), orient_axis_f, orient_axis_l);
        }
        face_vertex_indices.sort([&](uint a, uint b)->bool
        {
            return angles[a] < angles[b];
        });

        out_faces.Add({ face_vertex_indices.length(), out_indices.length() });
        out_indices.AddMany(face_vertex_indices);
    }
}

void GeometryEditor::compute_normals(const List<Mesh::Vertex>& vertices, const List<uint>& indices, List<Vector3>& out_normals, bool invert)
{
    out_normals.Clear();

    for (uint i = 0; i < indices.length() / 3; i++)
    {
        auto normal = compute_normal(vertices[indices[i * 3 + 0]].pos, vertices[indices[i * 3 + 1]].pos, vertices[indices[i * 3 + 2]].pos);
        if (invert) normal *= -1;
        out_normals.Add(normal);
    }
}

Shared<Mesh> GeometryEditor::get_unit_cube()
{
    static Shared<Mesh> result;
    if (result == nullptr)
    {
        List<Mesh::Vertex> vertices = {
            {{0, 0, 0}, {0, 1}, Vector3::one()}, {{0, 1, 0}, {1, 1}, Vector3::one()}, {{1, 0, 0}, {0, 0}, Vector3::one()},
            {{1, 0, 0}, {0, 0}, Vector3::one()}, {{0, 1, 0}, {1, 1}, Vector3::one()}, {{1, 1, 0}, {1, 0}, Vector3::one()}, 
        };

        translate(vertices, Vector3::one() * -0.5f);
        
        result = MakeShared<Mesh>("Unit Cube", vertices);
    }
    
    return result;
}
