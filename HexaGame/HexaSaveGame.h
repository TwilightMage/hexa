#pragma once

#include "ChunkIndex.h"
#include "TileIndex.h"
#include "Engine/Compound.h"
#include "Engine/Map.h"
#include "Engine/Pointers.h"
#include "Engine/SaveGame.h"

class TileInfo;

class EXPORT HexaSaveGame : public SaveGame
{
public:
    HexaSaveGame(const String& profile_name);

    Shared<Map<TileIndex, ConstPtr<TileInfo>>> get_chunk_modifications(const ChunkIndex& index) const;
    void save_chunk_modifications(const ChunkIndex& index, const Map<TileIndex, ConstPtr<TileInfo>>& modifications);
    Shared<Compound::Object> get_world_settings() const;
};
