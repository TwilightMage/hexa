#include "CollisionEditorWorld.h"

#include "Engine/EditorPlayer.h"
#include "Engine/Game.h"
#include "Engine/Texture.h"
#include "HexaGame/Paths.h"
#include "HexaGame/Entities/MeshEntity.h"

void CollisionEditorWorld::on_start()
{
    const auto player = MakeShared<EditorPlayer>();
    spawn_entity(player, Vector3(-2, 0, 0));
    Game::possess(player);

    const auto sample = MakeShared<MeshEntity>(Mesh::load_obj(RESOURCES_HEXA_MESHES + "arrow.obj"));
    sample->get_material_instance()->set_param_value("texture", Texture::load_png(RESOURCES_HEXA_TEXTURES + "arrow.png"));
    spawn_entity(sample);
}
