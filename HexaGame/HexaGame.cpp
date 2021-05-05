#include "HexaGame.h"



#include "Engine/Texture.h"
#include "Entities/Arrow.h"
#include "Entities/DebugPlayer.h"
#include "Entities/DemoMeshEntity.h"

HexaGame::HexaGame(int argc, char* argv[])
    : Game(argc, argv)
{
}

void HexaGame::init_game_info(GameInfo& outInfo)
{
    outInfo.title = "Hexa";
}

void HexaGame::start()
{
    List<uint> l = {1, 2, 3, 4};
    l.Insert(10, 2);
    
    lock_mouse();
    hide_mouse();
    
    auto world = MakeShared<World>();
    open_world(world);

    const auto player = MakeShared<DebugPlayer>();
    world->spawn_entity(player, Vector3(-3.0f, 0.0f, 0.0f), Quaternion(Vector3(0.0f, 0.0f, 0.0f)));
    possess(player);

    const auto entity1 = MakeShared<DemoMeshEntity>();
    entity1->name = "Tile 1";
    world->spawn_entity(entity1, Vector3(0.0f, 0.0f, 0.0f));
    
    const auto entity2 = MakeShared<DemoMeshEntity>();
    entity2->name = "Tile 2";
    world->spawn_entity(entity2, Vector3(0.0f, 0.86602540378444f * 2, 0.0f));
    entity2->use_texture(Texture::load_png("resources/hexagame/textures/tiles/grass.png"));

    const auto entity3 = MakeShared<DemoMeshEntity>();
    entity3->name = "Tile 3";
    world->spawn_entity(entity3, Vector3(0.0f, 0.86602540378444f * 4, 0.0f));
    entity3->use_texture(Texture::load_png("resources/hexagame/textures/tiles/sand.png"));

    const auto entity4 = MakeShared<DemoMeshEntity>();
    entity4->name = "Tile 4";
    world->spawn_entity(entity4, Vector3(0.0f, 0.86602540378444f * 6, 0.0f));
    entity4->use_texture(Texture::load_png("resources/hexagame/textures/tiles/stone.png"));

    const auto entity5 = MakeShared<DemoMeshEntity>();
    entity5->name = "Tile 5";
    world->spawn_entity(entity5, Vector3(0.0f, 0.86602540378444f * 8, 0.0f));
    entity5->use_texture(Texture::load_png("resources/hexagame/textures/tiles/dirt.png"));

    dump_texture_usage();
}
