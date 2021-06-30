#include "Mesh.h"

#include <OBJ-Loader/Source/OBJ_Loader.h>

#include "Assert.h"
#include "File.h"
#include "Game.h"
#include "GeometryEditor.h"

Shared<Mesh> Mesh::empty = MakeShared<Mesh>("Empty Mesh");

Mesh::Mesh(const String& name)
    : Object(name)
    , usage_count_(0)
{
}

Mesh::Mesh(const String& name, const List<Vertex>& vertices)
    : Object(name)
    , vertices_(vertices)
    , indices_(vertices.length())
    , usage_count_(0)
{
    for (uint i = 0; i < vertices.length(); i++)
    {
        indices_[i] = i;
    }

    GeometryEditor::compute_normals(vertices_, indices_, true);

    calculate_bbox();
}

Mesh::Mesh(const String& name, const List<Vertex>& vertices, const List<uint>& indices)
    : Object(name)
    , vertices_(vertices)
    , indices_(indices)
    , usage_count_(0)
{
    GeometryEditor::compute_normals(vertices_, indices_, true);
    
    calculate_bbox();
}

Shared<Mesh> Mesh::load_obj(const Path& path)
{
    if (const auto found = Game::instance_->meshes_.find(path.get_absolute_string()))
    {
        return *found;
    }

    if (!Check(path.exists(), "Mesh Loader", "Mesh does not exists %s", path.get_absolute_string().c())) return nullptr;
    
    Shared<objl::Loader> loader = MakeShared<objl::Loader>();
    if (!Check(loader->LoadFile(path.get_absolute().to_string().std()), "Mesh Loader", "Unknown error on loading mesh %s", path.get_absolute_string().c())) return nullptr;
    if (!Check(loader->LoadedVertices.size() > 0, "Mesh Loader", "Number of vertices is 0 in mesh %s", path.get_absolute_string().c())) return nullptr;
    if (!Check(loader->LoadedIndices.size() > 0, "Mesh Loader", "Number of indices is 0 in mesh %s", path.get_absolute_string().c())) return nullptr;
        
    loader->LoadedMeshes.clear();
        
    Shared<Mesh> result = MakeShared<Mesh>(path.get_absolute_string());
        
    for (auto& vert : loader->LoadedVertices)
    {
        result->vertices_.Add({cast_object<Vector3>(vert.Position), {vert.TextureCoordinate.X, 1 - vert.TextureCoordinate.Y}, Vector3::one(), cast_object<Vector3>(vert.Normal)});
    }
    loader->LoadedVertices.clear();

    result->indices_ = List(loader->LoadedIndices);
    loader->LoadedIndices.clear();

    GeometryEditor::invert_indices(result->indices_);
    GeometryEditor::remove_indices(result->vertices_, result->indices_);
    GeometryEditor::rotate(result->vertices_, Quaternion(Vector3(90.0f, 0.0f, 180.0f)));
    GeometryEditor::mirror_y(result->vertices_);
    //GeometryEditor::compute_normals(result->vertices_, result->indices_, true);

    result->calculate_bbox();

    Game::instance_->meshes_[path.get_absolute_string()] = result;
    verbose("Mesh", "Loaded mesh v%i i%i %s", result->vertices_.length(), result->indices_.length(), path.get_absolute_string().c());

    return result;
}

const List<Mesh::Vertex>& Mesh::get_vertices() const
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

void Mesh::calculate_bbox()
{
    float min_x = 0;
    float max_x = 0;
    float min_y = 0;
    float max_y = 0;
    float min_z = 0;
    float max_z = 0;

    if (vertices_.length() > 0)
    {
        min_x = max_x = vertices_.first().pos.x;
        min_y = max_y = vertices_.first().pos.y;
        min_z = max_z = vertices_.first().pos.z;
        
        for (auto& vertex : vertices_)
        {
            min_x = Math::min(min_x, vertex.pos.x);
            max_x = Math::max(max_x, vertex.pos.x);
            min_y = Math::min(min_y, vertex.pos.y);
            max_y = Math::max(max_y, vertex.pos.y);
            min_z = Math::min(min_z, vertex.pos.z);
            max_z = Math::max(max_z, vertex.pos.z);
        }
    }

    bbox_center_ = Vector3((max_x + min_x) / 2, (max_y + min_y) / 2, (max_z + min_z) / 2);
    bbox_half_size_ = Vector3((max_x - min_x) / 2, (max_y - min_y) / 2, (max_z - min_z) / 2);
}
