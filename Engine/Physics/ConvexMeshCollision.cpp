#include "ConvexMeshCollision.h"

#include <reactphysics3d/collision/PolygonVertexArray.h>
#include <reactphysics3d/engine/PhysicsCommon.h>

#include "Engine/Game.h"

ConvexMeshCollision::DataBlock::~DataBlock()
{
    get_physics()->destroyConvexMeshShape(convex_mesh_shape);
    get_physics()->destroyPolyhedronMesh(polyhedron_mesh);
    delete polygon_vertex_array;
}

ConvexMeshCollision::~ConvexMeshCollision()
{
    if (const auto data_block = data_blocks_.find(source_mesh_))
    {
        if (--(*data_block)->usage_count == 0)
        {
            data_blocks_.remove(source_mesh_);
        }
    }
}

ConvexMeshCollision::ConvexMeshCollision(const Shared<Mesh>& source_mesh)
    : source_mesh_(source_mesh)
{
    if (source_mesh)
    {
        auto& data_block = data_blocks_.find_or_insert(source_mesh_, [&]()->Shared<DataBlock>
        {
            Shared<DataBlock> result = MakeShared<DataBlock>();
            fill_data_block(result, source_mesh->vertices_, source_mesh->indices_);
            return result;
        });
        
        data_block->usage_count++;
    }
}

ConvexMeshCollision::ConvexMeshCollision(const ConvexMeshCollision& rhs)
    : source_mesh_(rhs.source_mesh_)
{
    if (const auto data_block = data_blocks_.find(source_mesh_))
    {
        (*data_block)->usage_count++;
    }
}

ConvexMeshCollision& ConvexMeshCollision::operator=(const ConvexMeshCollision& rhs)
{
    if (const auto data_block = data_blocks_.find(source_mesh_))
    {
        if (--(*data_block)->usage_count == 0)
        {
            data_blocks_.remove(source_mesh_);
        }
    }

    source_mesh_ = rhs.source_mesh_;
    
    if (const auto data_block = data_blocks_.find(source_mesh_))
    {
        (*data_block)->usage_count++;
    }

    return *this;
}

reactphysics3d::CollisionShape* ConvexMeshCollision::get_collider_shape() const
{
    if (const auto found = data_blocks_.find(source_mesh_)) return (*found)->convex_mesh_shape;

    return nullptr;
}

void ConvexMeshCollision::fill_data_block(const Shared<DataBlock>& data_block, List<Mesh::Vertex> source_vertices, List<uint> source_indices)
{
    GeometryEditor::optimize_collision(source_vertices, source_indices);
    
    GeometryEditor::compute_faces(source_vertices, source_indices, data_block->faces, data_block->indices);
    
    for (auto& vertex : source_vertices)
    {
        data_block->vertices.Add(vertex.pos);
    }

    data_block->polygon_vertex_array = new reactphysics3d::PolygonVertexArray(
        data_block->vertices.length(),
        data_block->vertices.get_data(),
        sizeof(Vector3),
        data_block->indices.get_data(),
        sizeof(uint),
        data_block->faces.length(),
        reinterpret_cast<reactphysics3d::PolygonVertexArray::PolygonFace*>(data_block->faces.get_data()),
        reactphysics3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
        reactphysics3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE
        );
    
    data_block->polyhedron_mesh = get_physics()->createPolyhedronMesh(data_block->polygon_vertex_array);
    
    data_block->convex_mesh_shape = get_physics()->createConvexMeshShape(data_block->polyhedron_mesh);
}
