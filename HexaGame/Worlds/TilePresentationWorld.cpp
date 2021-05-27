#include "TilePresentationWorld.h"

#include "Engine/GeometryEditor.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/WorldGenerator.h"
#include "HexaGame/Entities/DebugPlayer.h"
#include "HexaGame/Entities/TileDemoEntity.h"

void TilePresentationWorld::on_start()
{
    Game::lock_mouse();
    Game::hide_mouse();
    
    const auto player = MakeShared<DebugPlayer>();
    spawn_entity(player, Vector3(-3.0f, 0.0f, 0.0f), Quaternion(Vector3(0.0f, 0.0f, 0.0f)));
    Game::possess(player);

    uint x = 0;
    uint y = 0;
    uint i = 2;
    for (auto& entry : HexaGame::tile_database->records())
    {
        const Vector3 pos = TileIndex(x, y++, 0).to_vector();
        
        List<Mesh::vertex> vertices;
        List<uint> indices;
        WorldGenerator::generate_tile_mesh(TileSide::All, Tiles::grass.get(), vertices, indices, pos.x + pos.y + pos.z);
        GeometryEditor::remove_indices(vertices, indices);

        Shared<Mesh> mesh = MakeShared<Mesh>("tile", vertices);
        
        const auto entity = MakeShared<Entity>();
        entity->use_mesh(mesh);
        spawn_entity(entity, pos);
        entity->use_texture(entry.second->texture);

        if (i++ == 3)
        {
            x++;
            y--;
            i -= 3;
        }
    }

    const auto tile_demo_entity = MakeShared<TileDemoEntity>(std::array<Shared<const TileInfo>, 6>{Tiles::dirt.get(), Tiles::dirt.get(), Tiles::grass.get(), Tiles::dirt.get(), Tiles::grass.get(), Tiles::grass.get()});
    spawn_entity(tile_demo_entity, TileIndex(3, 0, 0).to_vector());
}
