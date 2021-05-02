#include "Mesh.h"

#include <OBJ-Loader/Source/OBJ_Loader.h>

#include "File.h"
#include "Game.h"
#include "GeometryEditor.h"

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

Mesh::Mesh()
    : usage_count_(0)
{
}

Mesh::Mesh(const List<vertex>& vertices)
    : vertices_(vertices)
    , usage_count_(0)
{
    for (uint i = 0; i < vertices.Length(); i++)
    {
        indices_.Add(i);
    }
}

Mesh::Mesh(const List<vertex>& vertices, const List<uint>& indices)
    : vertices_(vertices)
    , indices_(indices)
    , usage_count_(0)
{
}

Shared<Mesh> Mesh::load_obj(const Path& path)
{
    const auto found = Game::instance_->meshes_.find(path.get_absolute_string());
    if (found != Game::instance_->meshes_.end())
    {
        return found->second;
    }
    
    Shared<objl::Loader> loader = MakeShared<objl::Loader>();
    if (loader->LoadFile(path.get_absolute().to_string().std()))
    {
        loader->LoadedMeshes.clear();
        
        Shared<Mesh> result = MakeShared<Mesh>();

        for (auto& vert : loader->LoadedVertices)
        {
            result->vertices_.Add({{vert.Position.X, vert.Position.Y, vert.Position.Z}, {vert.TextureCoordinate.X, vert.TextureCoordinate.Y}, {1.0f, 1.0f, 1.0f}});
        }
        loader->LoadedVertices.clear();

        result->indices_ = List(loader->LoadedIndices);
        loader->LoadedIndices.clear();

        GeometryEditor::remove_indices(result->vertices_, result->indices_);
        GeometryEditor::rotate(result->vertices_, Quaternion(Vector3(90.0f, 0.0f, 90.0f)));
        GeometryEditor::mirror_y(result->vertices_);

        Game::instance_->meshes_[path.get_absolute_string()] = result;

        return result;
    }

    return nullptr;
}

const List<Mesh::vertex>& Mesh::get_vertices() const
{
    return vertices_;
}

const List<uint>& Mesh::get_indices() const
{
    return indices_;
}

uint Mesh::get_usage_count() const
{
    return usage_count_;
}
