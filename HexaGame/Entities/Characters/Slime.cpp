#include "Slime.h"

#include "Engine/Animation.h"
#include "Engine/World.h"
#include "HexaGame/Items.h"
#include "HexaGame/Paths.h"
#include "HexaGame/Entities/ItemDrop.h"

Slime::Slime()
{
    declared_height = 2;
    
    step_animation = { Animation::load_from_file(RESOURCES_HEXA_ANIMATIONS + "slime_step.anim"), false };
}

void Slime::on_start()
{
    use_mesh(Mesh::load_obj(RESOURCES_HEXA_MESHES_CHARACTERS + "slime.obj"));
    use_texture(Texture::load_png(RESOURCES_HEXA_TEXTURES_CHARACTERS + "slime.png"));

    get_world()->spawn_entity(MakeShared<ItemDrop>(ItemContainer(Items::iron_shovel)), TileIndex(0, 1, get_tile_position().z).to_vector() + Items::iron_shovel->mesh->get_bounds_half_size().x, Quaternion(Vector3(0, 90, 0)));
}

void Slime::tick(float delta_time)
{
    Character::tick(delta_time);
}
