#pragma once

#include "Collision.h"
#include "Engine/BasicTypes.h"
#include "Engine/GeometryEditor.h"
#include "Engine/List.h"
#include "Engine/Map.h"

namespace reactphysics3d
{
    class PolygonVertexArray;
    class ConvexMeshShape;
    class PolyhedronMesh;
}

class Mesh;

class EXPORT ConvexMeshCollision : public Collision
{
    struct DataBlock
    {
        ~DataBlock();
        
        List<Vector3> vertices;
        List<uint> indices;
        List<GeometryEditor::Face> faces;
        reactphysics3d::PolygonVertexArray* polygon_vertex_array;
        reactphysics3d::PolyhedronMesh* polyhedron_mesh;
        reactphysics3d::ConvexMeshShape* convex_mesh_shape;
        uint usage_count = 0;
    };
    
public:
    ~ConvexMeshCollision();
    explicit ConvexMeshCollision(const Shared<Mesh>& source_mesh);

    ConvexMeshCollision(const ConvexMeshCollision& rhs);

    ConvexMeshCollision& operator=(const ConvexMeshCollision& rhs);

protected:
    reactphysics3d::CollisionShape* get_collider_shape() const override;

private:
    static void fill_data_block(const Shared<DataBlock>& data_block, List<Mesh::Vertex> source_vertices, List<uint> source_indices);

    inline static Map<Shared<Mesh>, Shared<DataBlock>> data_blocks_ = Map<Shared<Mesh>, Shared<DataBlock>>();

    Shared<Mesh> source_mesh_;
};
