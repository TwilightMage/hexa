#include "DemoMeshEntity.h"

#include "Mesh.h"

void DemoMeshEntity::start()
{
    static float i = 0;
    
    Mesh* mesh = new Mesh();
    mesh->vertices = List<Mesh::vertex>({
        { -0.5f, -0.5f, i, 0.0f, 0.0f, 1.f, 0.f, 0.f },
        { 0.5f, -0.5f, i, 1.0f, 0.0f, 0.f, 1.f, 0.f },
        { -0.5f,  0.5f, i, 0.0f, 1.0f, 0.f, 0.f, 1.f }
    });

    i++;
    use_mesh(mesh);
}
