#include "HexaGame.h"



#include "Tiles.h"
#include "CollisionEditor/CollisionEditorWorld.h"
#include "Engine/Texture.h"
#include "Entities/Arrow.h"
#include "Entities/DebugPlayer.h"
#include "Entities/DemoMeshEntity.h"
#include "ui/TileDatabaseViewer.h"

Shared<Database<TileInfo>> HexaGame::tile_database = MakeShared<Database<TileInfo>>("Tile Database");

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
    lock_mouse();
    hide_mouse();
    
    auto world = MakeShared<World>();
    open_world(world);

    /*const auto dialogue_panel = MakeShared<DialoguePanel>();
    add_ui(dialogue_panel);*/

    const auto player = MakeShared<DebugPlayer>();
    world->spawn_entity(player, Vector3(-3.0f, 0.0f, 0.0f), Quaternion(Vector3(0.0f, 0.0f, 0.0f)));
    possess(player);

    const auto entity1 = MakeShared<DemoMeshEntity>();
    entity1->name = "Tile 1";
    world->spawn_entity(entity1, Vector3(0.0f, 0.0f, 0.0f));
    entity1->use_texture(Tiles::stone_bricks.get()->texture);
    
    const auto entity2 = MakeShared<DemoMeshEntity>();
    entity2->name = "Tile 2";
    world->spawn_entity(entity2, Vector3(0.0f, 0.86602540378444f * 2, 0.0f));
    entity2->use_texture(Tiles::grass.get()->texture);

    const auto entity3 = MakeShared<DemoMeshEntity>();
    entity3->name = "Tile 3";
    world->spawn_entity(entity3, Vector3(0.0f, 0.86602540378444f * 4, 0.0f));
    entity3->use_texture(Tiles::sand.get()->texture);

    const auto entity4 = MakeShared<DemoMeshEntity>();
    entity4->name = "Tile 4";
    world->spawn_entity(entity4, Vector3(0.0f, 0.86602540378444f * 6, 0.0f));
    entity4->use_texture(Tiles::stone.get()->texture);

    const auto entity5 = MakeShared<DemoMeshEntity>();
    entity5->name = "Tile 5";
    world->spawn_entity(entity5, Vector3(0.0f, 0.86602540378444f * 8, 0.0f));
    entity5->use_texture(Tiles::dirt.get()->texture);
}

void HexaGame::loading_stage()
{
    tile_database->register_entries<Tiles>();
}
