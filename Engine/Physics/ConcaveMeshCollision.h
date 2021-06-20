#pragma once

#include "Collision.h"
#include "Engine/BasicTypes.h"
#include "Engine/GeometryEditor.h"
#include "Engine/Map.h"

namespace reactphysics3d {
    class ConcaveMeshShape;
    class TriangleVertexArray;
    class TriangleMesh;
}

class EXPORT ConcaveMeshCollision : public Collision
{
    struct DataBlock
    {
        ~DataBlock();

        reactphysics3d::TriangleVertexArray* triangle_vertex_array;
        reactphysics3d::TriangleMesh* triangle_mesh;
        reactphysics3d::ConcaveMeshShape* concave_mesh_shape;
        uint usage_count = 0;
    };
    
public:
    ~ConcaveMeshCollision();
    
    explicit ConcaveMeshCollision(const Shared<Mesh>& source_mesh);
    ConcaveMeshCollision(const ConcaveMeshCollision& rhs);

    ConcaveMeshCollision& operator=(const ConcaveMeshCollision& rhs);

protected:
    reactphysics3d::CollisionShape* get_collider_shape() const override;

private:
    static void fill_data_block(const Shared<DataBlock>& data_block, const Shared<Mesh>& mesh);

    inline static Map<Shared<Mesh>, Shared<DataBlock>> data_blocks_ = Map<Shared<Mesh>, Shared<DataBlock>>();

    Shared<Mesh> source_mesh_;
};
