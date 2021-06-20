#include "Slime.h"

#include "HexaGame/Paths.h"

void Slime::on_start()
{
    use_mesh(Mesh::load_obj(RESOURCES_HEXA_MESHES_CHARACTERS + "slime.obj"));
    use_texture(Texture::load_png(RESOURCES_HEXA_TEXTURES_CHARACTERS + "slime.png"));
}
