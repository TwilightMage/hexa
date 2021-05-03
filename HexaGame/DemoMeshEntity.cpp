#include "DemoMeshEntity.h"

#include "Engine/Mesh.h"

void DemoMeshEntity::start()
{
    static Shared<Mesh> mesh = Mesh::load_obj("resources/hexagame/meshes/hexagon_tile.obj"); /*= Shared<Mesh>(new Mesh
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
}
