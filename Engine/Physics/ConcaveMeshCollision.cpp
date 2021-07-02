#include "ConcaveMeshCollision.h"

#include <reactphysics3d/collision/TriangleVertexArray.h>
#include <reactphysics3d/engine/PhysicsCommon.h>

ConcaveMeshCollision::DataBlock::~DataBlock()
{
    get_physics()->destroyConcaveMeshShape(concave_mesh_shape);
    get_physics()->destroyTriangleMesh(triangle_mesh);
    delete triangle_vertex_array;
}

ConcaveMeshCollision::~ConcaveMeshCollision()
{
    if (const auto data_block = data_blocks_.find(source_mesh_))
    {
        if (--(*data_block)->usage_count == 0)
        {
            data_blocks_.remove(source_mesh_);
        }
    }
}

ConcaveMeshCollision::ConcaveMeshCollision(const Shared<Mesh>& source_mesh)
    : source_mesh_(source_mesh)
{
    if (source_mesh)
    {
        auto& data_block = data_blocks_.find_or_insert(source_mesh_, [&]()->Shared<DataBlock>
        {
            Shared<DataBlock> result = MakeShared<DataBlock>();
            fill_data_block(result, source_mesh);
            return result;
        });
        
        data_block->usage_count++;
    }
}

ConcaveMeshCollision::ConcaveMeshCollision(const ConcaveMeshCollision& rhs)
    : source_mesh_(rhs.source_mesh_)
{
    if (const auto data_block = data_blocks_.find(source_mesh_))
    {
        (*data_block)->usage_count++;
    }
}

ConcaveMeshCollision& ConcaveMeshCollision::operator=(const ConcaveMeshCollision& rhs)
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

reactphysics3d::CollisionShape* ConcaveMeshCollision::get_collider_shape() const
{
    if (const auto found = data_blocks_.find(source_mesh_)) return (*found)->concave_mesh_shape;

    return nullptr;
}

void ConcaveMeshCollision::fill_data_block(const Shared<DataBlock>& data_block, const Shared<Mesh>& mesh)
{
    data_block->triangle_vertex_array = new reactphysics3d::TriangleVertexArray(
        mesh->get_vertices().length(),
        mesh->get_vertices().get_data(),
        sizeof(Mesh::Vertex),
        mesh->get_indices().length() / 3,
        mesh->get_indices().get_data(),
        sizeof(int) * 3,
        reactphysics3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
        reactphysics3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
        );

    reactphysics3d::Vector3 n1;
    data_block->triangle_vertex_array->getNormal(0, &n1);
    
    data_block->triangle_mesh = get_physics()->createTriangleMesh();
    data_block->triangle_mesh->addSubpart(data_block->triangle_vertex_array);
    
    data_block->concave_mesh_shape = get_physics()->createConcaveMeshShape(data_block->triangle_mesh);
}
