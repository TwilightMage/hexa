#include "HexaGame.h"



#include "Tiles.h"
#include "CollisionEditor/CollisionEditorWorld.h"
#include "Engine/Texture.h"
#include "Entities/Arrow.h"
#include "Entities/DebugPlayer.h"
#include "Entities/DemoMeshEntity.h"
#include "Entities/TileDemoMeshEntity.h"
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
    world->spawn_entity(entity1, TileIndex(0, 0, 0).to_vector());
    entity1->use_texture(Tiles::dirt.get()->texture);
    
    const auto entity2 = MakeShared<DemoMeshEntity>();
    world->spawn_entity(entity2, TileIndex(0, 1, 0).to_vector());
    entity2->use_texture(Tiles::grass.get()->texture);

    const auto entity3 = MakeShared<DemoMeshEntity>();
    world->spawn_entity(entity3, TileIndex(1, 1, 0).to_vector());
    entity3->use_texture(Tiles::sand.get()->texture);

    const auto entity4 = MakeShared<DemoMeshEntity>();
    world->spawn_entity(entity4, TileIndex(1, 2, 0).to_vector());
    entity4->use_texture(Tiles::stone.get()->texture);

    const auto entity5 = MakeShared<DemoMeshEntity>();
    world->spawn_entity(entity5, TileIndex(1, 3, 0).to_vector());
    entity5->use_texture(Tiles::iron_ore.get()->texture);

    const auto entity6 = MakeShared<DemoMeshEntity>();
    world->spawn_entity(entity6, TileIndex(2, 3, 0).to_vector());
    entity6->use_texture(Tiles::stone_bricks.get()->texture);

    const auto entity = MakeShared<TileDemoMeshEntity>(std::array<Shared<const TileInfo>, 6>{Tiles::dirt.get(), Tiles::dirt.get(), Tiles::grass.get(), Tiles::dirt.get(), Tiles::grass.get(), Tiles::grass.get()});
    world->spawn_entity(entity, Vector3(3, 0, 0));
}

void HexaGame::loading_stage()
{
    tile_database->register_entries<Tiles>();
}
