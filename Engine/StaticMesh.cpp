#include "StaticMesh.h"

#include <OBJ-Loader/OBJ_Loader.h>
#include <OGRE/Main/OgreHardwareBufferManager.h>
#include <OGRE/Main/OgreMesh.h>
#include <OGRE/Main/OgreMeshManager.h>
#include <OGRE/Main/OgreResourceGroupManager.h>
#include <OGRE/Main/OgreSubMesh.h>
#include <OGRE/Main/OgreVertexIndexData.h>

#include "Assert.h"
#include "File.h"
#include "Game.h"
#include "GeometryEditor.h"
#include "Physics/BoxCollision.h"
#include "Physics/ConcaveMeshCollision.h"
#include "Physics/SphereCollision.h"
#include "Physics/ConvexMeshCollision.h"

struct Bounds
{
    Vector3 min;
    Vector3 max;

    FORCEINLINE void add(const Vector3& point)
    {
        min.x = Math::min(min.x, point.x);
        min.y = Math::min(min.y, point.y);
        min.z = Math::min(min.z, point.z);
        max.x = Math::max(max.x, point.x);
        max.y = Math::max(max.y, point.y);
        max.z = Math::max(max.z, point.z);
    }

    FORCEINLINE Vector3 get_center() const { return (max + min) * 0.5f; };
    FORCEINLINE Vector3 get_extents() const { return (max - min) * 0.5f; };
};

Shared<StaticMesh> StaticMesh::empty = MakeShared<StaticMesh>("Empty Mesh");

StaticMesh::SubMesh::SubMesh(const String& name, const List<Vertex>& vertices, const List<uint>& indices)
    : name(name)
    , vertices(vertices)
    , indices(indices)
{
}

StaticMesh::SubMesh::SubMesh(const List<Vertex>& vertices, const List<uint>& indices)
    : SubMesh("", vertices, indices)
{
}

StaticMesh::SubMesh::SubMesh()
    : SubMesh("", {}, {})
{
}

void StaticMesh::SubMesh::add(const List<Vertex>& new_vertices, const List<uint>& new_indices)
{
    uint offset = vertices.length();
    vertices.add_many(new_vertices);
    for (auto& ind : new_indices)
    {
        indices.add(ind + offset);
    }
}

StaticMesh::StaticMesh(const String& name)
    : Object(name)
{
}

Shared<StaticMesh> StaticMesh::construct(const String& name, const List<SubMesh>& sub_meshes, AutoCollisionMode collision_mode)
{
    const auto result = create(name, sub_meshes, collision_mode, true);

    verbose("Mesh", "Constructed mesh %s", name.c());

    return result;
}

Shared<StaticMesh> StaticMesh::load_file_obj(const Path& path, AutoCollisionMode collision_mode)
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

    List<SubMesh> sub_meshes(loader->LoadedMeshes.size());
    for (uint i = 0; i < sub_meshes.length(); i++)
    {
        auto& sub_mesh = sub_meshes[i];
        const auto src_sub_mesh = loader->LoadedMeshes[i];
        sub_mesh.name = src_sub_mesh.MeshName;
        sub_mesh.vertices = List<Vertex>(src_sub_mesh.Vertices.size());
        for (uint j = 0; j < src_sub_mesh.Vertices.size(); j++)
        {
            const auto& vert = src_sub_mesh.Vertices[j];
            sub_mesh.vertices[j] = Vertex{
                cast_object<Vector3>(vert.Position) * 100,
                Vector2(vert.TextureCoordinate.X, 1 - vert.TextureCoordinate.Y),
                cast_object<Vector3>(vert.Normal)
            };
        }

        sub_mesh.indices = src_sub_mesh.Indices;

        GeometryEditor::optimize(sub_mesh.vertices, sub_mesh.indices);
        GeometryEditor::rotate(sub_mesh.vertices, Quaternion::from_axis_angle(Vector3::forward(), 90));
    }
    
    Shared<StaticMesh> result = create(path.filename + path.extension, sub_meshes, collision_mode, false);

    Game::instance_->meshes_[path.get_absolute_string()] = result;
    verbose("Mesh", "Loaded mesh %s", path.get_absolute_string().c());

    return result;
}

const Vector3& StaticMesh::get_bounds_center() const
{
    return cast_object<Vector3>(ogre_mesh_->getBounds().getCenter());
}

const Vector3& StaticMesh::get_bounds_half_size() const
{
    return cast_object<Vector3>(ogre_mesh_->getBounds().getHalfSize());
}

bool StaticMesh::is_empty() const
{
    return ogre_mesh_->sharedVertexData->vertexCount == 0;
}

Shared<StaticMesh> StaticMesh::create(const String& name, const List<SubMesh>& sub_meshes, AutoCollisionMode collision_mode, bool compute_normals)
{
    Shared<StaticMesh> result = MakeShared<StaticMesh>(name);

    result->ogre_mesh_ = Ogre::MeshManager::getSingleton().createManual(name.c(), Ogre::RGN_DEFAULT).get();

    result->ogre_mesh_->sharedVertexData = new Ogre::VertexData();

    Ogre::VertexDeclaration* decl = result->ogre_mesh_->sharedVertexData->vertexDeclaration;
    Ogre::VertexBufferBinding* bind = result->ogre_mesh_->sharedVertexData->vertexBufferBinding;
    
    size_t offset = 0;
    decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
    decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
    decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

    List<Vertex> vertices_copy;
    List<uint> indices_copy;

    uint vertex_offset = 0;
    Bounds visual_bounds;
    for (auto& sub_mesh : sub_meshes)
    {   
        if (sub_mesh.name.starts_with("SPHERE_")) // Sphere collision
        {
            if (collision_mode == AutoCollisionMode::Default)
            {
                Bounds sub_bounds;
                for (const auto& vert : sub_mesh.vertices)
                {
                    sub_bounds.add(cast_object<Vector3>(vert.pos));
                }

                result->collisions_.add(CollisionShapeInfo(sub_bounds.get_center(), Quaternion(), MakeShared<SphereCollision>(sub_bounds.get_extents().get_min_axis())));
            }
        }
        else if (sub_mesh.name.starts_with("BOX_")) // Box collision
        {
            if (collision_mode == AutoCollisionMode::Default)
            {
                Bounds sub_bounds;
                for (const auto& vert : sub_mesh.vertices)
                {
                    sub_bounds.add(cast_object<Vector3>(vert.pos));
                }

                result->collisions_.add(CollisionShapeInfo(sub_bounds.get_center(), Quaternion(), MakeShared<BoxCollision>(sub_bounds.get_extents())));
            }
        }
        else if (sub_mesh.name.starts_with("CONVEX_")) // Convex collision
        {
            if (collision_mode == AutoCollisionMode::Default)
            {
                auto vertices = sub_mesh.vertices;
                
                Bounds sub_bounds;
                for (const auto& vert : vertices)
                {
                    sub_bounds.add(cast_object<Vector3>(vert.pos));
                }

                const Vector3 sub_mesh_center = sub_bounds.get_center();
                if (sub_mesh_center != Vector3::zero())
                {
                    for (auto& vert : vertices)
                    {
                        vert.pos -= sub_mesh_center;
                    }
                }

                result->collisions_.add(CollisionShapeInfo(sub_mesh_center, Quaternion(), MakeShared<ConvexMeshCollision>(vertices, sub_mesh.indices)));
            }
        }
        else
        {
            auto vertices = sub_mesh.vertices;

            if (compute_normals)
            {
                GeometryEditor::compute_normals(vertices, sub_mesh.indices, true);
            }
            
            for (uint i = 0; i < vertices.length(); i++)
            {
                const auto& vert = vertices[i];
                vertices_copy.add(Vertex{ vert.pos, vert.uv, vert.norm });

                visual_bounds.add(cast_object<Vector3>(vert.pos));
            }

            Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(Ogre::HardwareIndexBuffer::IT_32BIT, sub_mesh.indices.length(), Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
            for (uint i = 0; i < sub_mesh.indices.length(); i++)
            {
                uint ind = sub_mesh.indices[i] + vertex_offset;
                if (collision_mode == AutoCollisionMode::Complex || collision_mode == AutoCollisionMode::Convex)
                {
                    indices_copy.add(ind);
                }
                ibuf->writeData(i * sizeof(uint), sizeof(uint), &ind, false);
            }

            auto sub = result->ogre_mesh_->createSubMesh();
            sub->useSharedVertices = true;
            sub->indexData->indexBuffer = ibuf;
            sub->indexData->indexCount = sub_mesh.indices.length();
            sub->indexData->indexStart = 0;
            
            vertex_offset += vertices.length();
        }
    }
    Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(offset, vertices_copy.length(), Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    for (uint i = 0; i < vertices_copy.length(); i++)
    {
        const auto& vert = vertices_copy[i];

        vbuf->writeData(i * (sizeof(Vector3) + sizeof(Vector2) + sizeof(Vector3)),                                     sizeof(Vector3), &vert.pos, false);
        vbuf->writeData(i * (sizeof(Vector3) + sizeof(Vector2) + sizeof(Vector3)) + sizeof(Vector3),                   sizeof(Vector2), &vert.uv, false);
        vbuf->writeData(i * (sizeof(Vector3) + sizeof(Vector2) + sizeof(Vector3)) + sizeof(Vector3) + sizeof(Vector2), sizeof(Vector3), &vert.norm, false);
    }
    bind->setBinding(0, vbuf);

    if (collision_mode == AutoCollisionMode::Complex)
    {
        result->collisions_.add(CollisionShapeInfo(Vector3::zero(), Quaternion(), MakeShared<ConcaveMeshCollision>(vertices_copy, indices_copy)));
    }
    else if (collision_mode == AutoCollisionMode::Convex)
    {
        result->collisions_.add(CollisionShapeInfo(Vector3::zero(), Quaternion(), MakeShared<ConvexMeshCollision>(vertices_copy, indices_copy)));
    }

    result->ogre_mesh_->_setBounds(Ogre::AxisAlignedBox(cast_object<Ogre::Vector3>(visual_bounds.min), cast_object<Ogre::Vector3>(visual_bounds.max)));
    result->ogre_mesh_->buildEdgeList();

    return result;
}
