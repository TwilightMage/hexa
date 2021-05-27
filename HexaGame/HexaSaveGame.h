#pragma once

#include <map>


#include "ChunkIndex.h"
#include "TileIndex.h"
#include "Engine/Path.h"
#include "Engine/SaveGame.h"

class TileInfo;

EXTERN class EXPORT HexaSaveGame : public SaveGame
{
public:
    HexaSaveGame(const String& profile_name);

    Shared<std::map<TileIndex, Shared<const TileInfo>>> get_chunk_modifications(const ChunkIndex& index) const;
};
