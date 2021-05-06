#include "CollisionEditorWorld.h"

#include "CollisionEditorPlayer.h"
#include "Engine/Game.h"
#include "Engine/Texture.h"
#include "HexaGame/Entities/MeshEntity.h"

void CollisionEditorWorld::on_start()
{
    const auto player = MakeShared<CollisionEditorPlayer>();
    spawn_entity(player, Vector3(-2, 0, 0));
    Game::possess(player);

    const auto sample = MakeShared<MeshEntity>(Mesh::load_obj("resources/hexagame/meshes/arrow.obj"));
    sample->use_texture(Texture::load_png("resources/hexagame/textures/arrow.png"));
    spawn_entity(sample);
    player->sample = sample;
}
