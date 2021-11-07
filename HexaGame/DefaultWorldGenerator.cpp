#include "DefaultWorldGenerator.h"

#include "HexaMath.h"
#include "TileIndex.h"
#include "WorldChunk.h"
#include "ComplexTileCustomData/TreeStemCustomData.h"
#include "Database/Tiles.h"
#include "Engine/Random.h"

void DefaultWorldGenerator::init(uint seed)
{
    srand(seed);
    std::array<byte, 256> permutation;
    Random random(seed);
    for (uint i = 0; i < permutation.size(); i++)
    {
        permutation[i] = random.number<byte>();
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
    //generate_debug(editable);
    //return;
    
    const auto chunk_world_position = editable.get_chunk()->get_index().to_vector();

    const float forest_h = (float)generator_.accumulatedOctaveNoise3D_0_1(chunk_world_position.x * 0.001f / 100.0f, chunk_world_position.y * 0.001f / 100.0f, 10, 4);
    const float plains_h = (float)generator_.accumulatedOctaveNoise3D_0_1(chunk_world_position.x * 0.001f / 100.0f, chunk_world_position.y * 0.001f / 100.0f, 11, 4);
    
    /*if (forest_h > plains_h)
    {
        generate_forest(editable);
    }
    else
    {
        generate_plains(editable);
    }*/
    generate_forest(editable);
}

void DefaultWorldGenerator::generate_plains(const EditableChunk& editable)
{
    const static TallGrassInfo* flowers[3] = {
        Tiles::blue_roses,
        Tiles::red_roses,
        Tiles::chamomile
    };

    const auto chunk = editable.get_chunk();
    const auto chunk_pos = chunk->get_index().to_vector();
    
    generate_land(editable);

    for (uint c_x = 0; c_x < WorldChunk::chunk_size; c_x++)
    {
        for (uint c_y = 0; c_y < WorldChunk::chunk_size; c_y++)
        {
            for (int c_z = WorldChunk::chunk_height - 1; c_z >= 0; c_z--)
            {
                auto tile_index = TileIndex(c_x, c_y, c_z);
                const Vector3 world_position = chunk_pos + tile_index.to_vector();

                auto& tile = editable.tile(tile_index);
                if (tile == Tiles::dirt)
                {
                    tile = Tiles::grass;

                    if (c_z < WorldChunk::chunk_height - 1 && (generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 0.04f, world_position.y * 0.04f, 1, 1) * 0.8f + 0.2f) * generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 2, world_position.y * 2, 2, 1) > 0.3f)
                    {
                        const double probs[3] = {
                            generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 0.08f / 100.0f, world_position.y * 0.08f / 100.0f, 2, 1) * generator_.accumulatedOctaveNoise3D_0_1(world_position.x / 100.0f, world_position.y / 100.0f, 3, 1),
                            generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 0.08f / 100.0f, world_position.y * 0.08f / 100.0f, 4, 1) * generator_.accumulatedOctaveNoise3D_0_1(world_position.x / 100.0f, world_position.y / 100.0f, 5, 1),
                            generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 0.08f / 100.0f, world_position.y * 0.08f / 100.0f, 6, 1) * generator_.accumulatedOctaveNoise3D_0_1(world_position.x / 100.0f, world_position.y / 100.0f, 7, 1)
                        };

                        uint max_prob = 0;
                        for (uint i = 1; i < sizeof(flowers) / sizeof(flowers[0]); i++)
                        {
                            if (probs[i] > probs[max_prob])
                            {
                                max_prob = i;
                            }
                        }
                        
                        if (probs[max_prob] > 0.35f)
                        {
                            editable.tile(tile_index.offset(0, 0, 1)) = flowers[max_prob];   
                        }
                        else
                        {
                            editable.tile(tile_index.offset(0, 0, 1)) = Tiles::tall_grass;
                        }
                    }
                    break;
                }
            }
        }
    }
}

void DefaultWorldGenerator::generate_forest(const EditableChunk& editable)
{
    const auto chunk = editable.get_chunk();
    const auto chunk_pos = chunk->get_index().to_vector();

    generate_land(editable);

    for (uint x = 0; x < WorldChunk::chunk_size; x++)
    {
        for (uint y = 0; y < WorldChunk::chunk_size; y++)
        {
            uint free_tiles = 0;
            for (int z = WorldChunk::chunk_height - 1; z >= 0; z--)
            {
                auto tile_index = TileIndex(x, y, z);
                const Vector3 world_position = chunk_pos + tile_index.to_vector();
                
                auto& tile = editable.tile(tile_index);
                if (tile != Tiles::air && tile != nullptr)
                {
                    if (tile == Tiles::dirt)
                    {
                        tile = Tiles::grass;

                        if (free_tiles >= 15 && generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 4 / 100.0f, world_position.y * 4 / 100.0f, 2, 1) > 0.62f)
                        {
                            uint tree_height = Random(tile_index.x + tile_index.y + tile_index.z).number(15, 20);
                            for (uint t = 1; t <= tree_height; t++)
                            {
                                TileIndex cell_index = tile_index.offset(0, 0, t);
                                editable.tile(cell_index) = Tiles::ash_stem;
                                if (auto custom_data = cast<TreeStemCustomData>(Tiles::ash_stem->create_custom_data()))
                                {
                                    if (t == 1) custom_data->type = TreeStemCustomData::Type::Roots;
                                    else if (t == tree_height) custom_data->type = TreeStemCustomData::Type::Top;
                                    else if (t > 2 && Random(x + y + z + t).number(6) == 0) custom_data->type = TreeStemCustomData::Type::Branched;
                                    custom_data->tree_seed = Random(x + y + z).number<uint>();
                                    custom_data->cell_index = t - 1;
                                    editable.set_complex_custom_data(cell_index, custom_data);
                                }
                            }
                        }
                        else if (free_tiles >= 1 && (generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 0.04f / 100.0f, world_position.y * 0.04f / 100.0f, 1, 1) * 0.8f + 0.2f) * generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 2 / 100.0f, world_position.y * 2 / 100.0f, 2, 1) > 0.3f)
                        {
                            editable.tile(tile_index.offset(0, 0, 1)) = Tiles::tall_grass;
                        }
                    }

                    break;
                }
                else
                {
                    free_tiles++;
                }
            }
        }
    }
}

void DefaultWorldGenerator::generate_debug(const EditableChunk& editable)
{
    for (uint x = 0; x < WorldChunk::chunk_size; x++)
    {
        for (uint y = 0; y < WorldChunk::chunk_size; y++)
        {
            editable.tile(x, y, 0) = Tiles::stone;
        }
    }

    if (editable.get_chunk()->get_index().x != 0 || editable.get_chunk()->get_index().y != 0) return;
    
    editable.tile(3, 3, 1) = Tiles::grass;
    editable.tile(3, 4, 1) = Tiles::dirt;
    editable.tile(3, 4, 2) = Tiles::grass;
    editable.tile(3, 4, 7) = Tiles::grass;
    editable.tile(3, 5, 4) = Tiles::grass;
    editable.tile(4, 5, 7) = Tiles::grass;
    editable.tile(4, 6, 7) = Tiles::grass;
}

void DefaultWorldGenerator::generate_land(const EditableChunk& editable) const
{
    const auto chunk_pos = editable.get_chunk()->get_index().to_vector();
    
    float heightmaps[10][10][2];
    for (uint x = 0; x < 10; x++)
    {
        for (uint y = 0; y < 10; y++)
        {
            const Vector3 world_position = chunk_pos + TileIndex(x, y, 0).to_vector();

            heightmaps[x][y][0] = (float)generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 0.01f / 100.0f, world_position.y * 0.01f / 100.0f, 0, 4) * ground_amplitude + ground_level;
            heightmaps[x][y][1] = heightmaps[x][y][0] - ((float)generator_.accumulatedOctaveNoise3D_0_1(world_position.x * 0.01f / 100.0f, world_position.y * 0.01f / 100.0f, 0, 4) * 2 + 2);
        }
    }
    
    for (uint x = 0; x < WorldChunk::chunk_size; x++)
    {
        for (uint y = 0; y < WorldChunk::chunk_size; y++)
        {
            for (uint z = 0; z < WorldChunk::chunk_height; z++)
            {
                const float Z = z * HexaMath::tile_height;
                if (Z < heightmaps[x][y][1])
                {
                    editable.tile(TileIndex(x, y, z)) = Tiles::stone;
                }
                else if (Z < heightmaps[x][y][0])
                {
                    editable.tile(TileIndex(x, y, z)) = Tiles::dirt;
                }
                else break;
            }
        }
    }
}
