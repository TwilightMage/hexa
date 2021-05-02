#include "GeometryEditor.h"

void GeometryEditor::optimize(List<Mesh::vertex>& vertices, List<uint>& indices)
{
    for (uint i = 0; i < vertices.Length() - 1; i++)
    {
        for (uint j = i + 1; j < vertices.Length(); j++)
        {
            if (memcmp(&vertices[i], &vertices[j], sizeof(Mesh::vertex)) == 0)
            {
                for (uint k = 0; k < indices.Length(); k++)
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
    for (uint i = 0; i < indices.Length() - 1; i++)
    {
        if (indices[i] != i)
        {
            vertices.Insert(vertices[indices[i]], i);
            indices[i] = i;
            for (uint j = i + 1; j < indices.Length(); j++)
            {
                if (indices[j] >= i) indices[j]++;
            }
        }
    }
    
    /*auto result = MakeShared<Mesh>();
    
    for (uint i = 0; i < indices_.Length(); i++)
    {
        result->vertices_.Add(vertices_[indices_[i]]);
        result->indices_.Add(i);
    }
    
    return result;*/
}

void GeometryEditor::invert_vertices(List<Mesh::vertex>& vertices)
{
    for (uint i = 0; i < vertices.Length(); i += 3)
    {
        std::swap(vertices[i], vertices[i + 2]);
    }
}

void GeometryEditor::invert_indices(List<uint>& indices)
{
    for (uint i = 0; i < indices.Length(); i += 3)
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
