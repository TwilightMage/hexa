#include "HexaSaveGame.h"


#include "HexaGame.h"
#include "Engine/File.h"

HexaSaveGame::HexaSaveGame(const String& profile_name)
    : SaveGame(profile_name)
{
}

Shared<std::map<TileIndex, Shared<const TileInfo>>> HexaSaveGame::get_chunk_modifications(const ChunkIndex& index) const
{
    const auto path_to_file = get_path().get_child("chunks").get_child(StringFormat("%i_%i", index.x, index.y));
    if (path_to_file.exists())
    {
        if (auto reader = File::Reader::open(path_to_file))
        {
            Shared<std::map<TileIndex, Shared<const TileInfo>>> result = MakeShared<std::map<TileIndex, Shared<const TileInfo>>>();

            while (!reader->is_end_of_file())
            {
                auto line = reader->read_line().split(' ');
                TileIndex tile_index = TileIndex(StringParse<int>(line[0]), StringParse<int>(line[1]), StringParse<int>(line[2]));
                if (const auto tile_id = HexaGame::tile_database->get(line[3]))
                {
                    result->operator[](tile_index) = tile_id;
                }
            }

            return result;
        }
    }
    
    return nullptr;
}
