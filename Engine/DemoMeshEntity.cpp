#include "DemoMeshEntity.h"

#include "Mesh.h"

void DemoMeshEntity::start()
{
    static Shared<Mesh> mesh = Mesh::load_obj("resources/engine/meshes/square.obj"); /*= Shared<Mesh>(new Mesh
    {
        List<Mesh::vertex>({
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.f, 0.f },
            { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.f, 0.f, 0.f },
            { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.f, 1.f, 0.f }
        })
    });*/

    use_mesh(mesh);
}
