#include "AxisArrows.h"

#include "Engine/StaticMesh.h"
#include "HexaGame/Paths.h"

AxisArrows::AxisArrows()
    : Entity()
{
}

void AxisArrows::on_start()
{
    static auto arrows_mesh = StaticMesh::load_file_obj(RESOURCES_MESHES + "axis_arrows.obj");
    //set_mesh(arrows_mesh);
}
