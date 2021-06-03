#include "ConvexMesh.h"

#include <reactphysics3d/collision/PolygonVertexArray.h>
#include <reactphysics3d/engine/PhysicsCommon.h>

#include "Game.h"

ConvexMesh::ConvexMesh()
    : data_block_id_(nullptr)
    , local_data_block_(nullptr)
{
}

ConvexMesh::ConvexMesh(const Shared<Mesh>& source_mesh)
    : data_block_id_(source_mesh)
    , local_data_block_(nullptr)
{
    if (!data_blocks_.contains(source_mesh))
    {
        fill_data_block(data_blocks_.find_or_insert(source_mesh), source_mesh->vertices_, source_mesh->indices_);
    }
}

ConvexMesh::ConvexMesh(const List<Mesh::Vertex>& vertices, const List<uint>& indices)
    : data_block_id_(nullptr)
    , local_data_block_(new DataBlock())
{
    fill_data_block(*local_data_block_, vertices, indices);
}

reactphysics3d::ConvexMeshShape* ConvexMesh::get_physics_shape() const
{
    if (local_data_block_) return local_data_block_->convex_mesh_shape;

    if (const auto found = data_blocks_.find(data_block_id_)) return found->convex_mesh_shape;

    return nullptr;
}

void ConvexMesh::fill_data_block(DataBlock& data_block, List<Mesh::Vertex> source_vertices, List<uint> source_indices)
{
    GeometryEditor::invert_vertices(source_vertices);
    GeometryEditor::optimize_collision(source_vertices, source_indices);
    
    GeometryEditor::generate_faces(source_vertices, source_indices, data_block.faces, data_block.indices);
    
    for (auto& vertex : source_vertices)
    {
        data_block.vertices.Add(vertex.pos);
    }

    data_block.polygon_vertex_array = new reactphysics3d::PolygonVertexArray(
        data_block.vertices.length(),
        data_block.vertices.get_data(),
        sizeof(Vector3),
        data_block.indices.get_data(),
        sizeof(uint),
        data_block.faces.length(),
        reinterpret_cast<reactphysics3d::PolygonVertexArray::PolygonFace*>(data_block.faces.get_data()),
        reactphysics3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
        reactphysics3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE
        );
    
    data_block.polyhedron_mesh = Game::instance_->physics_->createPolyhedronMesh(data_block.polygon_vertex_array);
    
    data_block.convex_mesh_shape = Game::instance_->physics_->createConvexMeshShape(data_block.polyhedron_mesh);
}
