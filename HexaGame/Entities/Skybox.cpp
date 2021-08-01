#include "Skybox.h"

#include "Engine/GeometryEditor.h"
#include "HexaGame/HexaGame.h"

Shared<StaticMesh> skybox_mesh;

void Skybox::on_start()
{
    /*if (skybox_mesh == nullptr)
    {
        auto vertices = GeometryEditor::get_unit_cube()->get_vertices();
        auto indices = GeometryEditor::get_unit_cube()->get_indices();

        GeometryEditor::invert_vertices(vertices);
        GeometryEditor::scale(vertices, Vector3(100));

        skybox_mesh = MakeShared<StaticMesh>("Skybox", vertices, indices);
    }
    
    set_mesh(skybox_mesh);
    set_material(HexaGame::skybox_material);*/
}
