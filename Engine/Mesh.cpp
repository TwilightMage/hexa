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
    for (uint i = 0; i < vertices.length(); i++)
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

    assert_error(path.exists(), nullptr, "Mesh Loader", "Mesh does not exists %s", path.get_absolute_string().c());
    
    Shared<objl::Loader> loader = MakeShared<objl::Loader>();
    assert_error(loader->LoadFile(path.get_absolute().to_string().std()), nullptr, "Mesh Loader", "Unknown error on loading mesh %s", path.get_absolute_string().c())
    assert_error(loader->LoadedVertices.size() > 0, nullptr, "Mesh Loader", "Number of vertices is 0 in mesh %s", path.get_absolute_string().c());
    assert_error(loader->LoadedIndices.size() > 0, nullptr, "Mesh Loader", "Number of indices is 0 in mesh %s", path.get_absolute_string().c());
        
    loader->LoadedMeshes.clear();
        
    Shared<Mesh> result = MakeShared<Mesh>();
        
    for (auto& vert : loader->LoadedVertices)
    {
        result->vertices_.Add({{vert.Position.X, vert.Position.Y, vert.Position.Z}, {vert.TextureCoordinate.X, 1 - vert.TextureCoordinate.Y}, {1.0f, 1.0f, 1.0f}});
    }
    loader->LoadedVertices.clear();

    result->indices_ = List(loader->LoadedIndices);
    loader->LoadedIndices.clear();

    GeometryEditor::remove_indices(result->vertices_, result->indices_);
    GeometryEditor::rotate(result->vertices_, Quaternion(Vector3(90.0f, 0.0f, 90.0f)));
    GeometryEditor::mirror_y(result->vertices_);

    Game::instance_->meshes_[path.get_absolute_string()] = result;
    verbose("Mesh Loader", "Loaded mesh v%i i%i %s", result->vertices_.length(), result->indices_.length(), path.get_absolute_string().c());

    return result;
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

bool Mesh::is_empty() const
{
    // TODO: Update after implementing index buffers
    return vertices_.length() == 0;// || indices_.length() == 0;
}
