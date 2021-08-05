#pragma once

#include "ChunkIndex.h"
#include "TileIndex.h"
#include "Engine/Map.h"
#include "Engine/Path.h"
#include "Engine/Pointers.h"
#include "Engine/SaveGame.h"

class JSON;
class TileInfo;

class EXPORT HexaSaveGame : public SaveGame
{
public:
    HexaSaveGame(const String& profile_name);

    Shared<Map<TileIndex, ConstPtr<TileInfo>>> get_chunk_modifications(const ChunkIndex& index) const;
    void save_chunk_modifications(const ChunkIndex& index, const Map<TileIndex, ConstPtr<TileInfo>>& modifications);
    Shared<JSON> get_world_settings() const;
};
