#include "DemoMeshEntity.h"

#include "Mesh.h"

void DemoMeshEntity::start()
{
    static float i;
    Shared<Mesh> mesh = Shared<Mesh>(new Mesh
    {
        List<Mesh::vertex>({
            { 0.0f, 0.0f, i, 0.0f, 0.0f, 0.f, 0.f, 0.f },
            { 1.0f, 0.0f, i, 1.0f, 0.0f, 1.f, 0.f, 0.f },
            { 0.0f, 1.0f, i, 0.0f, 1.0f, 0.f, 1.f, 0.f }
        })
    });
    i++;

    use_mesh(mesh);
}
