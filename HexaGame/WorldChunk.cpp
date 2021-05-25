#include "WorldChunk.h"

#include "WorldChunkData.h"
#include "WorldChunkMesh.h"
#include "Engine/World.h"

WorldChunk::WorldChunk(const Shared<WorldChunkData>& data)
    : data_(data)
{
}

void WorldChunk::make_visible()
{
    if (auto world = get_world().lock())
    {
        if (chunk_mesh_ == nullptr)
        {
            chunk_mesh_ = data_->generate_mesh();
        }
    
        for (auto& domain : chunk_mesh_->domains)
        {
            auto domain_mesh = MakeShared<Entity>();
            domain_mesh->set_position(get_position());
            domain_mesh->set_rotation(get_rotation());
            domain_mesh->set_scale(get_scale());
            domain_mesh->use_mesh(domain.mesh);
            domain_mesh->use_texture(domain.texture);
            world->spawn_entity(domain_mesh);

            child_domains_.Add(domain_mesh);
        }
    }
}