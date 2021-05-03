#include "AxisArrows.h"

#include "Engine/Mesh.h"

void AxisArrows::start()
{
    static auto arrows_mesh = Mesh::load_obj("resources/engine/meshes/axis_arrows.obj");
    use_mesh(arrows_mesh);
}
