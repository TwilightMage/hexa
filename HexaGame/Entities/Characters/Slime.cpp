#include "Slime.h"

#include "Engine/Animation.h"
#include "Engine/Mesh.h"
#include "Engine/World.h"
#include "HexaGame/Items.h"
#include "HexaGame/Paths.h"

Slime::Slime()
{
    declared_height = 2;
    basic_movement_speed = 1.25f;
    
    step_animation = { Animation::load_from_file(RESOURCES_HEXA_ANIMATIONS + "slime_step.anim"), false };
}

void Slime::on_start()
{
    Character::on_start();
    
    set_mesh(Mesh::load_obj(RESOURCES_HEXA_MESHES_CHARACTERS + "slime.obj"));
    get_material_instance()->set_param_value("texture", Texture::load_png(RESOURCES_HEXA_TEXTURES_CHARACTERS + "slime.png"));
}

void Slime::tick(float delta_time)
{
    Character::tick(delta_time);
}
