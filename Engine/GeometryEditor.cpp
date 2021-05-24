#include "GeometryEditor.h"

void GeometryEditor::optimize(List<Mesh::vertex>& vertices, List<uint>& indices)
{
    for (uint i = 0; i < vertices.length() - 1; i++)
    {
        for (uint j = i + 1; j < vertices.length(); j++)
        {
            if (memcmp(&vertices[i], &vertices[j], sizeof(Mesh::vertex)) == 0)
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

void GeometryEditor::remove_indices(List<Mesh::vertex>& vertices, List<uint>& indices)
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

    List<Mesh::vertex> result;
    for (auto index : indices)
    {
        result.Add(vertices[index]);
    }

    vertices = result;
}

void GeometryEditor::invert_vertices(List<Mesh::vertex>& vertices)
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

void GeometryEditor::randomize_colors(List<Mesh::vertex>& vertices)
{
}

void GeometryEditor::set_color(List<Mesh::vertex>& vertices, const Color& color)
{
    const auto vec_color = color.to_vector3();
    for (auto& vertex : vertices)
    {
        vertex.col = vec_color;
    }
}

void GeometryEditor::mirror_x(List<Mesh::vertex>& vertices)
{
    for (auto& vertex : vertices)
    {
        vertex.pos.x *= -1;
    }
}

void GeometryEditor::mirror_y(List<Mesh::vertex>& vertices)
{
    for (auto& vertex : vertices)
    {
        vertex.pos.y *= -1;
    }
}

void GeometryEditor::mirror_z(List<Mesh::vertex>& vertices)
{
    for (auto& vertex : vertices)
    {
        vertex.pos.z *= -1;
    }
}

void GeometryEditor::translate(List<Mesh::vertex>& vertices, const Vector3& offset)
{
    for (auto& vertex : vertices)
    {
        vertex.pos += offset;
    }
}

void GeometryEditor::rotate(List<Mesh::vertex>& vertices, const Quaternion& quat)
{
    for (auto& vertex : vertices)
    {
        vertex.pos = quat.rotate_vector3(vertex.pos);
    }
}

void GeometryEditor::scale(List<Mesh::vertex>& vertices, const Vector3& factor)
{
    for (auto& vertex : vertices)
    {
        vertex.pos *= factor;
    }
}

void GeometryEditor::move_to_center(List<Mesh::vertex>& vertices)
{
    Vector3 sum;
    for (const auto& vertex : vertices)
    {
        sum += vertex.pos;
    }
    sum /= static_cast<float>(vertices.length());
    translate(vertices, sum);
}

Shared<Mesh> GeometryEditor::get_unit_cube()
{
    static Shared<Mesh> result;
    if (result == nullptr)
    {
        List<Mesh::vertex> vertices = {
            {{0, 0, 0}, {0, 1}, Vector3::one()}, {{0, 1, 0}, {1, 1}, Vector3::one()}, {{1, 0, 0}, {0, 0}, Vector3::one()},
            {{1, 0, 0}, {0, 0}, Vector3::one()}, {{0, 1, 0}, {1, 1}, Vector3::one()}, {{1, 1, 0}, {1, 0}, Vector3::one()}, 
        };

        translate(vertices, Vector3::one() * -0.5f);
        
        result = MakeShared<Mesh>("Unit Cube", vertices);
    }
    
    return result;
}
