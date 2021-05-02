#include "DemoMeshEntity.h"

#include "Engine/Mesh.h"

void DemoMeshEntity::start()
{
    static Shared<Mesh> mesh = Shared<Mesh>(new Mesh
    {
        List<Mesh::vertex>({
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.f, 0.f, 0.f}},
            {{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.f, 0.f, 0.f}},
            {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.f, 1.f, 0.f}}
        })
    });

    use_mesh(mesh);
}
