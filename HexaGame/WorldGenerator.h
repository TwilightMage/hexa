#pragma once

#include <map>
#include <mutex>
#include <thread>

#include "EditableChunk.h"
#include "TileInfo.h"
#include "TileSide.h"
#include "Engine/BasicTypes.h"
#include "Engine/Compound.h"
#include "Engine/Map.h"
#include "Engine/StaticMesh.h"
#include "Engine/SimpleMap.h"

class EXPORT WorldGenerator
{
public:
    ~WorldGenerator();

    static void init_tile_variations();
    static void generate_tile_mesh(TileSide sides, ConstPtr<SolidTileInfo> tileInfo, List<StaticMesh::Vertex>& vertices, List<uint>& indices, float seed);

    void request_chunk_generation(const Shared<WorldChunk>& chunk);

    virtual void init(uint seed) = 0;
    
    virtual Compound::Object write_settings() const;
    virtual void read_settings(const Compound::Object& settings);

    void allocate_thread_pool();

protected:
    virtual void generate_chunk(const EditableChunk& editable) = 0;

private:
    struct PendingChunk
    {
        Shared<WorldChunk> chunk;
        bool is_taken;
    };
    
    void do_generate(const Shared<WorldChunk>& chunk);
    void finish_generation(const Shared<WorldChunk>& chunk);
    void thread_loop();
    
    List<PendingChunk> pending_chunks_;
    std::mutex pending_chunks_mutex_;
    List<Shared<std::thread>> thread_pool_;

    inline static bool stop = false;
};
