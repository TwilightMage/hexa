#include "DefaultWorldGenerator.h"

#include "TileIndex.h"
#include "Tiles.h"
#include "WorldChunk.h"
#include "Engine/Random.h"

#define rest()  std::this_thread::sleep_for(std::chrono::milliseconds(1));

void DefaultWorldGenerator::init(uint seed)
{
    srand(seed);
    std::array<byte, 256> permutation;
    for (uint i = 0; i < permutation.size(); i++)
    {
        permutation[i] = Random::global->number<byte>();
    }
    generator_.deserialize(permutation);
    generator_.reseed(seed);
}

JSON DefaultWorldGenerator::write_settings() const
{
    return JSON()
    .with_object("general", JSON()
        .with_float("ground_level", ground_level)
        .with_float("ground_amplitude", ground_amplitude)
        );
}

void DefaultWorldGenerator::read_settings(const JSON& settings)
{
    const auto general = settings.get_object("general");
    ground_level = general.get_float("ground_level", 30);
    ground_amplitude = general.get_float("ground_amplitude", 10);
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

            //rest();
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

            //rest();
        }
    }
}