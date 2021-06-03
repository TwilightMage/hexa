#pragma once

#include "BasicTypes.h"
#include "GeometryEditor.h"
#include "List.h"
#include "Map.h"

namespace reactphysics3d
{
    class PolygonVertexArray;
    class ConvexMeshShape;
    class PolyhedronMesh;
}

class Mesh;

class ConvexMesh
{
    struct DataBlock
    {
        List<Vector3> vertices;
        List<uint> indices;
        List<GeometryEditor::Face> faces;
        reactphysics3d::PolygonVertexArray* polygon_vertex_array;
        reactphysics3d::PolyhedronMesh* polyhedron_mesh;
        reactphysics3d::ConvexMeshShape* convex_mesh_shape;
        uint usage_count = 1;
    };
    
public:
    ConvexMesh();
    explicit ConvexMesh(const Shared<Mesh>& source_mesh);
    ConvexMesh(const List<Mesh::Vertex>& vertices, const List<uint>& indices);

    reactphysics3d::ConvexMeshShape* get_physics_shape() const;

private:
    static void fill_data_block(DataBlock& data_block, List<Mesh::Vertex> source_vertices, List<uint> source_indices);
    
    inline static Map<Shared<Mesh>, DataBlock> data_blocks_ = Map<Shared<Mesh>, DataBlock>();

    Shared<Mesh> data_block_id_;
    Unique<DataBlock> local_data_block_;
};
