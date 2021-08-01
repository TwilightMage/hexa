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
    friend World;
    
public:    
    ConcaveMeshCollision(const List<StaticMesh::Vertex>& vertices, const List<uint>& indices);

    ~ConcaveMeshCollision();
    
    ConcaveMeshCollision& operator=(const ConcaveMeshCollision& rhs);

protected:
    reactphysics3d::CollisionShape* get_collider_shape() const override;

private:
    List<Vector3> vertices_copy_;
    List<uint> indices_copy_;
    reactphysics3d::TriangleVertexArray* triangle_vertex_array;
    reactphysics3d::TriangleMesh* triangle_mesh;
    reactphysics3d::ConcaveMeshShape* concave_mesh_shape;
};
