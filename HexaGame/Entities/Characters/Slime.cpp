#include "Slime.h"

void Slime::on_start()
{
    use_mesh(Mesh::load_obj("resources/hexagame/meshes/characters/slime.obj"));
    use_texture(Texture::load_png("resources/hexagame/textures/characters/slime.png"));
}
