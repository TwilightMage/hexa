#include "HexaGame.h"



#include "DefaultWorldGenerator.h"
#include "HexaWorld.h"
#include "Tiles.h"
#include "WorldChunk.h"
#include "WorldGenerator.h"
#include "CollisionEditor/CollisionEditorWorld.h"
#include "Engine/GeometryEditor.h"
#include "Engine/Texture.h"
#include "Entities/Arrow.h"
#include "Entities/DebugPlayer.h"
#include "Entities/DemoMeshEntity.h"
#include "Entities/TileDemoEntity.h"
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
    
    auto world = MakeShared<HexaWorld>(MakeShared<DefaultWorldGenerator>());
    open_world(world);

    /*const auto dialogue_panel = MakeShared<DialoguePanel>();
    add_ui(dialogue_panel);*/

    const auto player = MakeShared<DebugPlayer>();
    world->spawn_entity(player, Vector3(-3.0f, 0.0f, 0.0f), Quaternion(Vector3(0.0f, 0.0f, 0.0f)));
    possess(player);

    uint x = 0;
    uint y = 0;
    uint i = 2;
    for (auto& entry : tile_database->records())
    {
        const Vector3 pos = TileIndex(x, y++, 0).to_vector();
        
        List<Mesh::vertex> vertices;
        List<uint> indices;
        WorldGenerator::generate_tile_mesh(TileSide::All, Tiles::grass.get(), vertices, indices, pos.x + pos.y + pos.z);
        GeometryEditor::remove_indices(vertices, indices);

        Shared<Mesh> mesh = MakeShared<Mesh>("tile", vertices);
        
        const auto entity = MakeShared<Entity>();
        entity->use_mesh(mesh);
        world->spawn_entity(entity, pos);
        entity->use_texture(entry.second->texture);

        if (i++ == 3)
        {
            x++;
            y--;
            i -= 3;
        }
    }

    const auto tile_demo_entity = MakeShared<TileDemoEntity>(std::array<Shared<const TileInfo>, 6>{Tiles::dirt.get(), Tiles::dirt.get(), Tiles::grass.get(), Tiles::dirt.get(), Tiles::grass.get(), Tiles::grass.get()});
    world->spawn_entity(tile_demo_entity, TileIndex(3, 0, 0).to_vector());

    const auto world_chunk = MakeShared<WorldChunk>(world->get_generator()->generate_chunk(0, 0));
    world->spawn_entity(world_chunk, TileIndex(15, 0, 0).to_vector());
    world_chunk->make_visible();
}

void HexaGame::loading_stage()
{
    tile_database->register_entries<Tiles>();
}
