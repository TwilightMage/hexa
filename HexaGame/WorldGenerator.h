#pragma once

#include <thread>

#include "EditableChunk.h"
#include "TileInfo.h"
#include "TileSide.h"
#include "Engine/BasicTypes.h"
#include "Engine/Mesh.h"
#include "mini/ini.h"

class WorldChunkData;

EXTERN class EXPORT WorldGenerator
{
public:
    static void generate_tile_mesh(TileSide sides, const Shared<const TileInfo>& tileInfo, List<Mesh::vertex>& vertices, List<uint>& indices, float seed);

    void request_chunk_generation(const Shared<WorldChunkData>& chunk);

    virtual void init(uint seed) = 0;
    
    virtual mINI::INIStructure write_settings() const;
    virtual void read_settings(const mINI::INIStructure& settings);

protected:
    virtual void perform_chunk_generation(const EditableChunk& editable) = 0;

private:
    void try_to_start_new_generation();
    void do_generate(const Shared<WorldChunkData>& chunk);
    void finish_generation(const Shared<WorldChunkData>& chunk);
    
    List<Shared<WorldChunkData>> pending_chunks_;
    std::map<Shared<WorldChunkData>, Shared<std::thread>> threads_;
};
