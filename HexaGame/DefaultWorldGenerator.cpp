#include "DefaultWorldGenerator.h"

#include "TileIndex.h"
#include "Tiles.h"
#include "WorldChunkData.h"

Shared<WorldChunkData> DefaultWorldGenerator::generate_chunk(int x, int y)
{
    Shared<WorldChunkData> result = MakeShared<WorldChunkData>();
    
    for (uint c_x = 0; c_x < WorldChunkData::chunk_size; c_x++)
    {
        for (uint c_y = 0; c_y < WorldChunkData::chunk_size; c_y++)
        {
            for (uint c_z = 0; c_z < WorldChunkData::chunk_height; c_z++)
            {
                Vector3 world_position = TileIndex(x * WorldChunkData::chunk_size + c_x, y * WorldChunkData::chunk_size + c_y, c_z).to_vector();

                if (world_position.z < 32)
                {
                    result->data[c_x][c_y][c_z] = Tiles::dirt.get();
                }
            }
        }
    }

    for (uint c_x = 0; c_x < WorldChunkData::chunk_size; c_x++)
    {
        for (uint c_y = 0; c_y < WorldChunkData::chunk_size; c_y++)
        {
            for (uint c_z = WorldChunkData::chunk_height - 1; c_z >= 0; c_z--)
            {
                auto& tile = result->data[c_x][c_y][c_z];
                if (tile == Tiles::dirt.get())
                {
                    tile = Tiles::grass.get();
                    break;
                }
            }
        }
    }

    result->validate_and_generate_metadata();

    return result;
}
