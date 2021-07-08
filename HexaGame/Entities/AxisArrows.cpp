#include "AxisArrows.h"

#include "Engine/Mesh.h"
#include "HexaGame/Paths.h"

AxisArrows::AxisArrows()
    : Entity()
{
}

void AxisArrows::on_start()
{
    static auto arrows_mesh = Mesh::load_obj(RESOURCES_HEXA_MESHES + "axis_arrows.obj");
    set_mesh(arrows_mesh);
}
