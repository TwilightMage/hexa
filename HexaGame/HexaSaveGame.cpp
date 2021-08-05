#include "HexaSaveGame.h"


#include "HexaGame.h"
#include "WorldChunk.h"
#include "Database/Tiles.h"
#include "Engine/File.h"
#include "Engine/JSON.h"

HexaSaveGame::HexaSaveGame(const String& profile_name)
    : SaveGame(profile_name)
{
}

template<typename T>
FORCEINLINE T read(std::ifstream& stream)
{
    T result;
    stream.read((char*)&result, sizeof(T));
    return result;
}

template<typename T>
FORCEINLINE void write(std::ofstream& stream, T value)
{
    stream.write((char*)&value, sizeof(T));
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
            
            const int count = read<int>(stream);
            for (int i = 0; i < count; i++)
            {
                const auto x = read<byte>(stream);
                const auto y = read<byte>(stream);
                const auto z = read<byte16>(stream);

                String name;
                while (true)
                {
                    const char ch = read<char>(stream);
                    if (ch == '\0') break;
                    name += ch;
                }

                if (const auto tile_id = HexaGame::tile_database->get(Name(name)))
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
        write(stream, static_cast<int>(modifications.size()));

        for (auto& kvp : modifications)
        {
            write(stream, static_cast<byte>(kvp.key.x));
            write(stream, static_cast<byte>(kvp.key.y));
            write(stream, static_cast<byte16>(kvp.key.z));

            for (auto& ch : kvp.value->key.to_string())
            {
                write(stream, ch);
            }
            write(stream, '\0');
        }
    }
    stream.close();
}

Shared<JSON> HexaSaveGame::get_world_settings() const
{
    const auto path_to_file = get_path().get_child("world").get_child("world");
    if (path_to_file.exists())
    {
        return JSON::try_parse(File::read_file(path_to_file));
    }

    return nullptr;
}
