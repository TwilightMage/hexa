#include "Slime.h"

#include "Engine/Animation.h"
#include "HexaGame/Paths.h"

Shared<Animation> step_anim;

void Slime::on_start()
{
    if (!step_anim)
    {
        step_anim = Animation::load_from_file(RESOURCES_HEXA_ANIMATIONS + "slime_step.anim");
    }
    
    step_animation = { step_anim, false };
    
    use_mesh(Mesh::load_obj(RESOURCES_HEXA_MESHES_CHARACTERS + "slime.obj"));
    use_texture(Texture::load_png(RESOURCES_HEXA_TEXTURES_CHARACTERS + "slime.png"));
}
