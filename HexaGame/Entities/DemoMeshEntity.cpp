﻿#include "DemoMeshEntity.h"


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
    List<Mesh::Vertex> vertices;
    List<uint> indices;
    WorldGenerator::generate_tile_mesh(TileSide::All, Tiles::grass, vertices, indices, 0.0f);
    GeometryEditor::remove_indices(vertices, indices);

    Shared<Mesh> mesh = MakeShared<Mesh>("tile", vertices);
    
    set_mesh(mesh);
}
