#include "DemoMeshEntity.h"


#include "Engine/GeometryEditor.h"
#include "Engine/Mesh.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/WorldGenerator.h"

DemoMeshEntity::DemoMeshEntity()
    : Entity()
{
}

void DemoMeshEntity::on_start()
{
    List<Mesh::vertex> vertices;
    List<uint> indices;
    WorldGenerator::generate_tile_mesh(TileSide::All, Tiles::grass.get(), vertices, indices, 0.0f);
    GeometryEditor::remove_indices(vertices, indices);

    Shared<Mesh> mesh = MakeShared<Mesh>("tile", vertices);
    
    use_mesh(mesh);
}
