#include "WorldChunk.h"


#include "HexaWorld.h"
#include "WorldChunkData.h"
#include "WorldChunkMesh.h"
#include "WorldGenerator.h"
#include "Engine/World.h"

WorldChunk::WorldChunk(const ChunkIndex& index, const Weak<HexaWorld>& world)
    : index_(index)
    , world_(world)
{
}

void WorldChunk::load()
{
    if (auto world = world_.lock())
    {
        if (!data_)
        {
            data_ = MakeShared<WorldChunkData>(index_);
            world->get_generator()->request_chunk_generation(data_);
        }
    }
}

#define test_neighbor(candidate)\
    if (candidate->get_state() != WorldChunkDataState::Loaded)\
    {\
        candidate->on_loaded.bind_unique(this, &WorldChunk::loaded_callback);\
    }\
    else\
    {\
        load_counter_--;\
    }\


void WorldChunk::set_visibility(bool visibility)
{
    const bool old_state = visibility_counter_ > 0;
    visibility_counter_ += visibility ? 1 : -1;
    if (visibility_counter_ < 0)
    {
        visibility_counter_ = 0;
    }
    const bool new_state = visibility_counter_ > 0;

    if (old_state != new_state)
    {
        if (auto world = world_.lock())
        {
            if (new_state)
            {
                load_counter_ = 7;
                test_neighbor(data_);
                test_neighbor(data_->get_front());
                test_neighbor(data_->get_back());
                test_neighbor(data_->get_right());
                test_neighbor(data_->get_left());
                test_neighbor(data_->get_front_right());
                test_neighbor(data_->get_back_left());

                try_show_mesh();
            }
            else
            {
                chunk_mesh_ = nullptr;
                for (auto& domain : child_domains_)
                {
                    domain->destroy();
                }
                child_domains_.Clear();
            }
        }
    }
}

#undef test_neighbor

void WorldChunk::try_show_mesh()
{
    if (load_counter_ == 0)
    {
        if (auto world = world_.lock())
        {
            chunk_mesh_ = data_->generate_mesh();
            
            for (auto& domain : chunk_mesh_->domains)
            {
                auto domain_mesh = MakeShared<Entity>();
                domain_mesh->set_position(index_.to_vector());
                domain_mesh->use_mesh(domain.mesh);
                domain_mesh->use_texture(domain.texture);
                world->spawn_entity(domain_mesh);
        
                child_domains_.Add(domain_mesh);
            }
        }
    }
}

void WorldChunk::loaded_callback(const Shared<WorldChunkData>& sender)
{
    sender->on_loaded.unbind(this, &WorldChunk::loaded_callback);
    load_counter_--;
    try_show_mesh();
}
