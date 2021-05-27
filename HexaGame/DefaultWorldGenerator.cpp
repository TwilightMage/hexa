#include "DefaultWorldGenerator.h"

#include "HexaSaveGame.h"
#include "TileIndex.h"
#include "Tiles.h"
#include "WorldChunkData.h"

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

void DefaultWorldGenerator::perform_chunk_generation(const EditableChunk& editable)
{
    auto chunk = editable.get_chunk();

    Vector3 chunk_pos = chunk->get_index().to_vector();

    for (uint c_x = 0; c_x < WorldChunkData::chunk_size; c_x++)
    {
        for (uint c_y = 0; c_y < WorldChunkData::chunk_size; c_y++)
        {
            for (uint c_z = 0; c_z < WorldChunkData::chunk_height; c_z++)
            {
                Vector3 world_position = TileIndex(chunk->get_index().x * WorldChunkData::chunk_size + c_x, chunk->get_index().y * WorldChunkData::chunk_size + c_y, c_z).to_vector() + chunk_pos;

                if (world_position.z < generator_.accumulatedOctaveNoise2D_0_1(world_position.x * 0.01f, world_position.y * 0.01f, 4) * ground_amplitude + ground_level)
                {
                    editable.tile(TileIndex(c_x, c_y, c_z)) = Tiles::dirt.get();
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
                auto& tile = editable.tile(TileIndex(c_x, c_y, c_z));
                if (tile == Tiles::dirt.get())
                {
                    tile = Tiles::grass.get();
                    break;
                }
            }
        }
    }
}