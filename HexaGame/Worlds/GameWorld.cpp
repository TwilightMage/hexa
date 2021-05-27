﻿#include "GameWorld.h"


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
    
    auto observer = register_chunk_observer(ChunkIndex(0, 0), 2);
    observer->set_render_chunks(true);

    auto chunk = get_chunk(ChunkIndex(0, 0));
    if (chunk->get_data()->get_state() == WorldChunkDataState::Loaded)
    {
        spawn_player(chunk->get_data());
    }
    else
    {
        chunk->get_data()->on_loaded.bind(this, &GameWorld::spawn_player);
    }
}

void GameWorld::spawn_player(const Shared<WorldChunkData>& sender)
{
    sender->on_loaded.unbind(this, &GameWorld::spawn_player);

    for (uint i = 0; i < WorldChunkData::chunk_height; i++)
    {
        if (sender->get_tile(TileIndex(0, 0, WorldChunkData::chunk_height - 1 - i)) != Tiles::air.get())
        {
            const auto player = MakeShared<DebugPlayer>();
            spawn_entity(player, TileIndex(0, 0, WorldChunkData::chunk_height - 1 - i + 1).to_vector() + Vector3(0, 0, HexaMath::tile_height / 2));
            Game::possess(player);
            break;
        }
    }
}
