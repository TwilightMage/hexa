﻿#pragma once

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

class StaticMesh;

class EXPORT ConvexMeshCollision : public Collision
{
    friend World;
public:
    ConvexMeshCollision(const List<StaticMesh::Vertex>& vertices, const List<uint>& indices);
    ~ConvexMeshCollision();
    
protected:
    reactphysics3d::CollisionShape* get_collider_shape() const override;

private:
    List<Vector3> vertices_copy_;
    List<uint> indices_copy_;
    List<GeometryEditor::Face> faces;
    reactphysics3d::PolygonVertexArray* polygon_vertex_array;
    reactphysics3d::PolyhedronMesh* polyhedron_mesh;
    reactphysics3d::ConvexMeshShape* convex_mesh_shape;
};
