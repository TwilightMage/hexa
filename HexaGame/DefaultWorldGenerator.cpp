#include "DefaultWorldGenerator.h"

#include "HexaSaveGame.h"
#include "TileIndex.h"
#include "Tiles.h"
#include "WorldChunk.h"

#define rest()  std::this_thread::sleep_for(std::chrono::milliseconds(1));

void DefaultWorldGenerator::init(uint seed)
{
    srand(seed);
    std::array<byte, 256> permutation;
    for (uint i = 0; i < permutation.size(); i++)
    {
        permutation[i] = rand() % std::numeric_limits<byte>::max();
    }
    generator_.deserialize(permutation);
    generator_.reseed(seed);
}

mINI::INIStructure DefaultWorldGenerator::write_settings() const
{
    mINI::INIStructure result;
    result["general"].set("ground_level", StringMake(ground_level).std());

    return result;
}

void DefaultWorldGenerator::read_settings(const mINI::INIStructure& settings)
{
    ground_level = StringParse<float>(settings.get("general").get("ground_level"));
}

void DefaultWorldGenerator::generate_chunk(const EditableChunk& editable)
{
    const auto chunk = editable.get_chunk();

    const auto chunk_pos = chunk->get_index().to_vector();
    
    for (uint c_x = 0; c_x < WorldChunk::chunk_size; c_x++)
    {
        for (uint c_y = 0; c_y < WorldChunk::chunk_size; c_y++)
        {
            for (uint c_z = 0; c_z < WorldChunk::chunk_height; c_z++)
            {
                const Vector3 world_position = chunk_pos + TileIndex(c_x, c_y, c_z).to_vector();

                if (world_position.z < generator_.accumulatedOctaveNoise2D_0_1(world_position.x * 0.01f, world_position.y * 0.01f, 4) * ground_amplitude + ground_level)
                {
                    editable.tile(TileIndex(c_x, c_y, c_z)) = Tiles::dirt;
                }
            }

            rest();
        }
    }

    for (uint c_x = 0; c_x < WorldChunk::chunk_size; c_x++)
    {
        for (uint c_y = 0; c_y < WorldChunk::chunk_size; c_y++)
        {
            for (uint c_z = WorldChunk::chunk_height - 1; c_z >= 0; c_z--)
            {
                auto& tile = editable.tile(TileIndex(c_x, c_y, c_z));
                if (tile == Tiles::dirt)
                {
                    tile = Tiles::grass;
                    break;
                }
            }

            rest();
        }
    }
}