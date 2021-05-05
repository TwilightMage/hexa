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
    scale = Vector3::one() * 0.75f;
}

void Arrow::tick(float delta_time)
{
    const auto rot_axis = rotation.forward().cross_product(Vector3::up());
    rotation = rotation.rotate_around(rot_axis, delta_time * -10);
    position += rotation.forward() * delta_time * 10;
}
