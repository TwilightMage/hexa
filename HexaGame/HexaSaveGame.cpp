#include "HexaSaveGame.h"


#include "HexaGame.h"
#include "WorldChunk.h"
#include "Database/Tiles.h"
#include "Engine/Compound.h"
#include "Engine/File.h"

HexaSaveGame::HexaSaveGame(const String& profile_name)
    : SaveGame(profile_name)
{
}

Shared<Map<TileIndex, ConstPtr<TileInfo>>> HexaSaveGame::get_chunk_modifications(const ChunkIndex& index) const
{
    const auto path_to_file = get_path().get_child("world").get_child(String::format("%i_%i", index.x, index.y));
    if (path_to_file.exists())
    {        
        std::ifstream stream(path_to_file.get_absolute_string().c(), std::ios::in | std::ios::binary);
        if (stream.is_open())
        {
            Shared<Map<TileIndex, ConstPtr<TileInfo>>> result = MakeShared<Map<TileIndex, ConstPtr<TileInfo>>>();
            
            const int count = StreamUtils::read<int>(stream);
            for (int i = 0; i < count; i++)
            {
                const auto x = StreamUtils::read<byte>(stream);
                const auto y = StreamUtils::read<byte>(stream);
                const auto z = StreamUtils::read<byte16>(stream);

                ModuleAssetID name = ModuleAssetID(StreamUtils::read<String>(stream));

                if (const auto tile_id = HexaGame::get_instance()->get_tile_data(name))
                {
                    result->insert(TileIndex(x, y, z), tile_id);
                }
            }

            return result;
        }
    }
    
    return nullptr;
}

void HexaSaveGame::save_chunk_modifications(const ChunkIndex& index, const Map<TileIndex, ConstPtr<TileInfo>>& modifications)
{
    const auto path_to_file = get_path().get_child("world").get_child(String::format("%i_%i", index.x, index.y));
    File::write_file(path_to_file);

    std::ofstream stream(path_to_file.get_absolute_string().c(), std::ios::out | std::ios::binary);
    if (stream.is_open())
    {
        StreamUtils::write(stream, static_cast<int>(modifications.size()));

        for (auto& kvp : modifications)
        {
            StreamUtils::write(stream, static_cast<byte>(kvp.key.x));
            StreamUtils::write(stream, static_cast<byte>(kvp.key.y));
            StreamUtils::write(stream, static_cast<byte16>(kvp.key.z));

            StreamUtils::write(stream, kvp.value->get_id().to_string());
        }
    }
    stream.close();
}

Shared<Compound::Object> HexaSaveGame::get_world_settings() const
{
    const auto path_to_file = get_path().get_child("world").get_child("world");
    if (path_to_file.exists())
    {
        return MakeShared<Compound::Object>(Compound::Converters::JSON().parse_value(File::read_file(path_to_file)));
    }

    return nullptr;
}
