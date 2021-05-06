#include "Arrow.h"

#include "Engine/Mesh.h"
#include "Engine/Texture.h"

Arrow::Arrow()
    : Entity()
{
}

void Arrow::on_start()
{
    use_mesh(Mesh::load_obj("resources/hexagame/meshes/arrow.obj"));
    use_texture(Texture::load_png("resources/hexagame/textures/arrow.png"));
    //scale_ = Vector3::one() * 0.75f;
    use_sphere_collision(1.0f);
}

void Arrow::tick(float delta_time)
{
    //const auto rot_axis = rotation_.forward().cross_product(Vector3::up());
    //rotation_ = rotation_.rotate_around(rot_axis, delta_time * -10);
    //position_ += rotation_.forward() * delta_time * 10;
}

bool Arrow::is_rigid_body()
{
    return true;
}
