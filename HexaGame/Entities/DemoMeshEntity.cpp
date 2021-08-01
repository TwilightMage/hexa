#include "DemoMeshEntity.h"


#include "Engine/GeometryEditor.h"
#include "Engine/StaticMesh.h"
#include "HexaGame/WorldGenerator.h"
#include "HexaGame/Database/Tiles.h"

DemoMeshEntity::DemoMeshEntity()
    : Entity()
{
}

void DemoMeshEntity::on_start()
{
    List<StaticMesh::Vertex> vertices;
    List<uint> indices;
    WorldGenerator::generate_tile_mesh(TileSide::All, Tiles::grass, vertices, indices, 0.0f);
    GeometryEditor::remove_indices(vertices, indices);

    //Shared<StaticMesh> mesh = MakeShared<StaticMesh>("tile", vertices);
    
    //set_mesh(mesh);
}
