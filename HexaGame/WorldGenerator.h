#pragma once

#include <map>
#include <thread>

#include "EditableChunk.h"
#include "TileInfo.h"
#include "TileSide.h"
#include "Engine/BasicTypes.h"
#include "Engine/JSON.h"
#include "Engine/Mesh.h"

EXTERN class EXPORT WorldGenerator
{
public:
    ~WorldGenerator();
    
    static void generate_tile_mesh(TileSide sides, const Shared<const TileInfo>& tileInfo, List<Mesh::Vertex>& vertices, List<uint>& indices, float seed);

    void request_chunk_generation(const Shared<WorldChunk>& chunk);

    virtual void init(uint seed) = 0;
    
    virtual JSON write_settings() const;
    virtual void read_settings(const JSON& settings);

protected:
    virtual void generate_chunk(const EditableChunk& editable) = 0;

private:
    void try_to_start_new_generation();
    void do_generate(const Shared<WorldChunk>& chunk);
    void finish_generation(const Shared<WorldChunk>& chunk);
    
    List<Shared<WorldChunk>> pending_chunks_;
    std::map<Shared<WorldChunk>, Shared<std::thread>> threads_;
};
