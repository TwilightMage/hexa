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
    for (uint i = 0; i < indices.length() - 1; i++)
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
    }
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

void GeometryEditor::set_color(List<Mesh::vertex>& vertices, const Vector3& color) // TODO: Replace Vector3 with color
{
    for (auto& vertex : vertices)
    {
        vertex.col = color;
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
