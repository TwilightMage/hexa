#include "Mesh.h"

#include <OBJ-Loader/Source/OBJ_Loader.h>

#include "File.h"
#include "Game.h"

struct Triangle
{
    int a, b, c;
};

struct Vec3
{
    float x, y, z;
};

struct Vec2
{
    float x, y;
};

Shared<Mesh> Mesh::load_obj(const Path& path)
{
    Shared<objl::Loader> loader = MakeShared<objl::Loader>();
    if (loader->LoadFile(path.get_absolute().to_string().std()))
    {
        loader->LoadedMeshes.clear();
        
        Shared<Mesh> result = MakeShared<Mesh>();

        for (auto& vert : loader->LoadedVertices)
        {
            result->vertices.Add({{vert.Position.X, vert.Position.Y, vert.Position.Z}, {vert.TextureCoordinate.X, vert.TextureCoordinate.Y}, {1.0f, 1.0f, 1.0f}});
        }
        loader->LoadedVertices.clear();

        result->indices = List(loader->LoadedIndices);
        loader->LoadedIndices.clear();

        //result->optimize();
        result = result->remove_indices();
        result->invert();

        Game::instance_->meshes_.Add(result);

        return result;
    }

    return nullptr;
}

void Mesh::optimize()
{
    for (uint i = 0; i < vertices.Length() - 1; i++)
    {
        for (uint j = i + 1; j < vertices.Length(); j++)
        {
            if (memcmp(&vertices[i], &vertices[j], sizeof(vertex)) == 0)
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

void Mesh::set_color(const Vector3& rhs)
{
    for (auto& vertex : vertices)
    {
        vertex.col = rhs;
    }
}

void Mesh::invert()
{
    for (uint i = 0; i < vertices.Length(); i += 3)
    {
        std::swap(vertices[i], vertices[i + 2]);
    }
}

Shared<Mesh> Mesh::remove_indices() const
{
    auto result = MakeShared<Mesh>();
    
    for (uint i = 0; i < indices.Length(); i++)
    {
        result->vertices.Add(vertices[indices[i]]);
        result->indices.Add(i);
    }
    
    return result;
}

uint Mesh::get_usage_count() const
{
    return usage_count_;
}
