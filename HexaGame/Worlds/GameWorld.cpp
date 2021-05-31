#include "GameWorld.h"


#include "Engine/Tree1D.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/HexaMath.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/WorldChunk.h"
#include "HexaGame/Entities/DebugPlayer.h"

GameWorld::GameWorld(const Shared<WorldGenerator>& generator)
    : HexaWorld(generator)
{
}

void GameWorld::on_start()
{
    Game::lock_mouse();
    Game::hide_mouse();

    player_ = MakeShared<DebugPlayer>();
    spawn_entity(player_);

    const auto spawn_chunk = get_chunk(ChunkIndex::from_vector(player_->get_position()));
    if (spawn_chunk->get_data()->get_state() == WorldChunkDataState::Loaded)
    {
        spawn_chunk_loaded(spawn_chunk->get_data());
    }
    else
    {
        spawn_chunk->get_data()->on_loaded.bind(this, &GameWorld::spawn_chunk_loaded);
    }
}

void GameWorld::spawn_chunk_loaded(const Shared<WorldChunkData>& sender)
{
    sender->on_loaded.unbind(this, &GameWorld::spawn_chunk_loaded);

    for (uint i = 0; i < WorldChunkData::chunk_height; i++)
    {
        if (sender->get_tile(TileIndex(0, 0, WorldChunkData::chunk_height - 1 - i)) != Tiles::air)
        {
            player_->set_position(TileIndex(0, 0, WorldChunkData::chunk_height - 1 - i + 1).to_vector() + Vector3(0, 0, HexaMath::tile_height / 2));
            Game::possess(player_);
            break;
        }
    }
}
