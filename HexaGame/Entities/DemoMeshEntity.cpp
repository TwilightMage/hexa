#include "DemoMeshEntity.h"


#include "Engine/GeometryEditor.h"
#include "Engine/Mesh.h"
#include "Engine/Texture.h"

DemoMeshEntity::DemoMeshEntity()
    : Entity()
{
}

void DemoMeshEntity::on_start()
{
    //static Shared<Mesh> mesh = GeometryEditor::get_unit_cube();
    static Shared<Mesh> mesh = Mesh::load_obj("resources/hexagame/meshes/hexagon_tile.obj");
    /*static Shared<Mesh> mesh = Shared<Mesh>(new Mesh
    {
        List<Mesh::vertex>({
            {{0.f, 0.f, 0.f}, {0.f, 1.f}, {1.f, 1.f, 1.f}},
            {{0.f, 1.f, 0.f}, {1.f, 1.f}, {1.f, 1.f, 1.f}},
            {{1.f, 0.f, 0.f}, {0.f, 0.f}, {1.f, 1.f, 1.f}},
            {{1.f, 0.f, 0.f}, {0.f, 0.f}, {1.f, 1.f, 1.f}},
            {{0.f, 1.f, 0.f}, {1.f, 1.f}, {1.f, 1.f, 1.f}},
            {{1.f, 1.f, 0.f}, {1.f, 0.f}, {1.f, 1.f, 1.f}}
        })
    });*/

    use_mesh(mesh);

    use_texture(Texture::load_png("resources/hexagame/textures/tiles/stone_bricks.png"));
}
