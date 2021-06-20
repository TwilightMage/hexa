#include "TilePresentationWorld.h"

#include "Engine/GeometryEditor.h"
#include "Engine/Physics/ConvexMeshCollision.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/WorldGenerator.h"
#include "HexaGame/Entities/DebugPlayer.h"
#include "HexaGame/Entities/SingleTile.h"
#include "HexaGame/Entities/TileDemoEntity.h"

void TilePresentationWorld::on_start()
{
    const auto player = MakeShared<DebugPlayer>();
    spawn_entity(player, Vector3(-3.0f, 0.0f, 0.0f), Quaternion(Vector3(0.0f, 0.0f, 0.0f)));
    Game::possess(player);

    uint x = 0;
    uint y = 0;
    uint i = 2;
    for (auto& entry : HexaGame::tile_database->records())
    {
        const Vector3 pos = TileIndex(x, y++, 0).to_vector();
        
        List<Mesh::Vertex> vertices;
        List<uint> indices;
        WorldGenerator::generate_tile_mesh(TileSide::All, entry.second, vertices, indices, pos.x + pos.y + pos.z);
        GeometryEditor::remove_indices(vertices, indices);

        Shared<Mesh> mesh = MakeShared<Mesh>(String::format("tile %s", entry.second->key.c()), vertices);
        
        const auto entity = MakeShared<SingleTile>();
        entity->use_mesh(mesh);
        spawn_entity(entity, pos);
        entity->use_collision(MakeShared<ConvexMeshCollision>(mesh));
        entity->use_texture(entry.second->texture);

        if (i++ == 3)
        {
            x++;
            y--;
            i -= 3;
        }
    }

    const auto tile_demo_entity = MakeShared<TileDemoEntity>(std::array<Shared<const TileInfo>, 6>{Tiles::dirt, Tiles::dirt, Tiles::grass, Tiles::dirt, Tiles::grass, Tiles::grass});
    spawn_entity(tile_demo_entity, TileIndex(3, 0, 0).to_vector());
}
