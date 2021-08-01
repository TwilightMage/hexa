#include "Arrow.h"

#include "Engine/StaticMesh.h"
#include "Engine/Texture.h"
#include "Engine/Physics/BoxCollision.h"
#include "HexaGame/Paths.h"

Arrow::Arrow()
    : Entity()
{
}

void Arrow::on_start()
{
    //set_mesh(StaticMesh::load_file_obj(RESOURCES_HEXA_MESHES + "arrow.obj"));
    //get_material_instance()->set_param_value("texture", Texture::load_png(RESOURCES_HEXA_TEXTURES + "arrow.png"));
    //scale_ = Vector3::one() * 0.75f;
    set_collision(MakeShared<BoxCollision>(Vector3(0.5f, 0.05f, 0.05f)));
}

void Arrow::on_tick(float delta_time)
{
    //const auto rot_axis = rotation_.forward().cross_product(Vector3::up());
    //rotation_ = rotation_.rotate_around(rot_axis, delta_time * -10);
    //position_ += rotation_.forward() * delta_time * 10;
    //translate(get_rotation().forward() * delta_time * 10);
}
