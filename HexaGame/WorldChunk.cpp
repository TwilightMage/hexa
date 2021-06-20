#include "WorldChunk.h"



#include "ChunkMeshEntity.h"
#include "HexaSaveGame.h"
#include "HexaWorld.h"
#include "Tiles.h"
#include "WorldChunkMesh.h"
#include "WorldGenerator.h"
#include "Engine/GeometryEditor.h"
#include "Engine/World.h"
#include "Engine/Physics/ConcaveMeshCollision.h"

WorldChunk::WorldChunk(const ChunkIndex& index, const Weak<HexaWorld>& world)
    : index_(index)
    , world_(world)
    , state_(WorldChunkDataState::Pending)
    , plane_metadata{}
{
}

int WorldChunk::get_observe_counter() const
{
    return observe_counter_;
}

int WorldChunk::get_visibility_counter() const
{
    return visibility_counter_;
}

WorldChunk* WorldChunk::get_front() const
{
	return front_;
}

WorldChunk* WorldChunk::get_front_right() const
{
	return front_right_;
}

WorldChunk* WorldChunk::get_right() const
{
	return right_;
}

WorldChunk* WorldChunk::get_back() const
{
	return back_;
}

WorldChunk* WorldChunk::get_back_left() const
{
	return back_left_;
}

WorldChunk* WorldChunk::get_left() const
{
	return left_;
}

#undef get_tile
#undef try_get_tile

const ChunkIndex& WorldChunk::get_index() const
{
	return index_;
}

WorldChunkDataState WorldChunk::get_state() const
{
	return state_;
}

const Shared<const TileInfo>& WorldChunk::get_tile(const TileIndex& index) const
{
	return data[index.x][index.y][index.z];
}

void WorldChunk::set_tile(const TileIndex& index, const Shared<const TileInfo>& tile)
{
    modifications_[index] = tile;
    data[index.x][index.y][index.z] = tile;
    dirty_ = true;
}

TileSide WorldChunk::get_tile_face_flags(const TileIndex& tile_index) const
{
    auto flags = TileSide::None;
    const auto current_type = data[tile_index.x][tile_index.y][tile_index.z]->type;

    if (front_tile(tile_index)->type < current_type) flags |= TileSide::Front;
    if (front_right_tile(tile_index)->type < current_type) flags |= TileSide::FrontRight;
    if (back_right_tile(tile_index)->type < current_type) flags |= TileSide::BackRight;
    if (back_tile(tile_index)->type < current_type) flags |= TileSide::Back;
    if (back_left_tile(tile_index)->type < current_type) flags |= TileSide::BackLeft;
    if (front_left_tile(tile_index)->type < current_type) flags |= TileSide::FrontLeft;

    if (up_tile(tile_index)->type < current_type) flags |= TileSide::Up;
    if (down_tile(tile_index)->type < current_type) flags |= TileSide::Down;

    return flags;
}

void WorldChunk::save_if_dirty()
{
    if (dirty_)
    {
        if (auto save_game = cast<HexaSaveGame>(Game::get_save_game()))
        {
            save_game->save_chunk_modifications(index_, modifications_);
            dirty_ = false;
        }
    }
}

void WorldChunk::load()
{
    if (auto world = world_.lock())
    {
        world->get_generator()->request_chunk_generation(shared_from_this());
    }
}

void WorldChunk::try_show_mesh()
{
    if (load_counter_ == 0 && visibility_counter_ > 0)
    {
        regenerate_mesh();
    }
}

void WorldChunk::loaded_callback(const Shared<WorldChunk>& sender)
{
    sender->on_loaded.unbind(this, &WorldChunk::loaded_callback);
    load_counter_--;
    try_show_mesh();
}

void WorldChunk::set_state(WorldChunkDataState state)
{
    if (state_ == state) return;

    state_ = state;
    on_state_changed(shared_from_this(), state);
    if (state == WorldChunkDataState::Loading) on_loading(shared_from_this());
    else if (state == WorldChunkDataState::Loaded) on_loaded(shared_from_this());
}

void WorldChunk::link(WorldChunk* front, WorldChunk* right, WorldChunk* front_right,
    WorldChunk* back, WorldChunk* left, WorldChunk* back_left)
{
    if (front) front_ = front;
    if (right) right_ = right;
    if (front_right) front_right_ = front_right;
    if (back) back_ = back;
    if (left) left_ = left;
    if (back_left) back_left_ = back_left;

    if (front_) front_->back_ = this;
    if (right_) right_->left_ = this;
    if (front_right_) front_right_->back_left_ = this;
    if (back_) back_->front_ = this;
    if (left_) left_->right_ = this;
    if (back_left_) back_left_->front_right_ = this;
}

void WorldChunk::unlink()
{
    if (front_) front_->back_ = nullptr;
    if (right_) right_->left_ = nullptr;
    if (front_right_) front_right_->back_left_ = nullptr;
    if (back_) back_->front_ = nullptr;
    if (left_) left_->right_ = nullptr;
    if (back_left_) back_left_->front_right_ = nullptr;

    front_ = nullptr;
    right_ = nullptr;
    front_right_ = nullptr;
    back_ = nullptr;
    left_ = nullptr;
    back_left_ = nullptr;
}

bool WorldChunk::inc_observe()
{
    return observe_counter_++ == 0;
}

bool WorldChunk::dec_observe()
{
    if (--observe_counter_ == 0)
    {
        unlink();
        return true;
    }
	
    return false;
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

void WorldChunk::inc_visibility()
{
    if (visibility_counter_++ == 0)
    {
        load_counter_ = 7;
        test_neighbor(this);
        test_neighbor(get_front());
        test_neighbor(get_back());
        test_neighbor(get_right());
        test_neighbor(get_left());
        test_neighbor(get_front_right());
        test_neighbor(get_back_left());

        try_show_mesh();
    }
}

#undef test_neighbor

void WorldChunk::dec_visibility()
{
    if (--visibility_counter_ == 0)
    {
        for (auto& chunk_mesh : mesh_entities_)
        {
            chunk_mesh->destroy();
        }
        mesh_entities_.Clear();
    }
}

void WorldChunk::generate_metadata()
{
    for (uint x = 0; x < chunk_size; x++)
    {
        for (uint y = 0; y < chunk_size; y++)
        {
            for (uint z = 0; z < chunk_height; z++)
            {
                if (data[x][y][z] == nullptr)
                {
                    data[x][y][z] = Tiles::air;
                }
				
                plane_metadata[z] |= data[x][y][z]->type;
            }
        }
    }
}

FORCEINLINE byte count_difference(TileType flags)
{
	int int_flags = static_cast<int>(flags);
	byte count = 0;
	while (int_flags) {
		count += int_flags & 1;
		int_flags >>= 1;
	}

	// if Liquid or Air present, complex does not count
	if ((!!(flags & TileType::Liquid) || !!(flags & TileType::Air)) && !!(flags & TileType::Complex)) --count;
	
	return count;
}

void WorldChunk::regenerate_mesh()
{
    for (auto mesh_entity : mesh_entities_)
    {
        mesh_entity->destroy();
    }
    mesh_entities_.Clear();
    
    if (auto world = world_.lock())
    {
        std::map<Shared<const TileInfo>, List<Mesh::Vertex>> vertices;

        for (int z = 0; z < chunk_height; z++)
        {
            const TileType plane_metadata_front_right = front_right_->plane_metadata[z];
            const TileType plane_metadata_back_left = back_left_->plane_metadata[z];
            const TileType plane_metadata_right = right_->plane_metadata[z];
            const TileType plane_metadata_left = left_->plane_metadata[z];
            const TileType plane_metadata_forward = front_->plane_metadata[z];
            const TileType plane_metadata_back = back_->plane_metadata[z];
            const TileType plane_metadata_down = z > 0 ? plane_metadata[z - 1] : TileType::Air;
            const TileType plane_metadata_up = z < chunk_height - 1 ? plane_metadata[z + 1] : TileType::Air;
            if (count_difference(plane_metadata[z] | plane_metadata_up | plane_metadata_down | plane_metadata_forward | plane_metadata_back | plane_metadata_right | plane_metadata_left | plane_metadata_front_right | plane_metadata_back_left) >= 2)
            {
                for (int x = 0; x < chunk_size; x++)
                {
                    for (int y = 0; y < chunk_size; y++)
                    {
                        const auto index = TileIndex(x, y, z);
                        const auto& tile = data[x][y][z];

                        if (tile->type == TileType::Solid)
                        {
                            TileSide side_flags = get_tile_face_flags(index);

                            if (side_flags != TileSide::None)
                            {
                                Vector3 world_pos = index.to_vector();
                                List<Mesh::Vertex> tile_vertices;
                                List<uint> tile_indices;
                                WorldGenerator::generate_tile_mesh(side_flags, tile, tile_vertices, tile_indices, world_pos.sum_all());

                                GeometryEditor::remove_indices(tile_vertices, tile_indices);
                                GeometryEditor::translate(tile_vertices, world_pos);

                                vertices[tile] += tile_vertices;
                            }
                        }
                    }
                }
            }
        }

        for (auto& kvp : vertices)
        {
            auto mesh = MakeShared<Mesh>(String::format("Chunk {%i %i} %s", index_.x, index_.y, kvp.first->key.c()), kvp.second);
            
            auto mesh_entity = MakeShared<ChunkMeshEntity>();
            mesh_entity->use_mesh(mesh);
            mesh_entity->use_texture(kvp.first->texture);
            world->spawn_entity(mesh_entity, index_.to_vector());

            mesh_entity->use_collision(MakeShared<ConcaveMeshCollision>(mesh));
        
            mesh_entities_.Add(mesh_entity);
        }
    }
}

#define get_tile()			(data[pos.x][pos.y][pos.z])
#define try_get_tile(name)	(name ? name->data[pos.x][pos.y][pos.z] : Tiles::air)

const Shared<const TileInfo>& WorldChunk::front_tile(const TileIndex& index) const
{
	const auto pos = index.offset(1, 0, 0).cycle_chunk();

	return index.x < chunk_size - 1
        ? get_tile()
        : try_get_tile(front_);
}

const Shared<const TileInfo>& WorldChunk::front_right_tile(const TileIndex& index) const
{
	const auto pos = index.offset(1, 1, 0).cycle_chunk();

	return index.y < chunk_size - 1
        ? index.x < chunk_size - 1
            ? get_tile()
            : try_get_tile(front_)
        : index.x < chunk_size - 1
            ? try_get_tile(right_)
            : try_get_tile(front_right_);
}

const Shared<const TileInfo>& WorldChunk::back_right_tile(const TileIndex& index) const
{
	const auto pos = index.offset(0, 1, 0).cycle_chunk();

	return index.y < chunk_size - 1
        ? get_tile()
        : try_get_tile(right_);
}

const Shared<const TileInfo>& WorldChunk::back_tile(const TileIndex& index) const
{
	const auto pos = index.offset(-1, 0, 0).cycle_chunk();

	return index.x > 0
        ? get_tile()
        : try_get_tile(back_);
}

const Shared<const TileInfo>& WorldChunk::back_left_tile(const TileIndex& index) const
{
	const auto pos = index.offset(-1, -1, 0).cycle_chunk();

	return index.y > 0
        ? index.x > 0
            ? get_tile()
            : try_get_tile(back_)
        : index.x > 0
            ? try_get_tile(left_)
            : try_get_tile(back_left_);
}

const Shared<const TileInfo>& WorldChunk::front_left_tile(const TileIndex& index) const
{
	const auto pos = index.offset(0, -1, 0).cycle_chunk();

	return index.y > 0
        ? get_tile()
        : try_get_tile(left_);
}

const Shared<const TileInfo>& WorldChunk::up_tile(const TileIndex& index) const
{
	if (index.z == chunk_height - 1) return Tiles::air;
	
	const auto pos = index.offset(0, 0, 1);

	return get_tile();
}

const Shared<const TileInfo>& WorldChunk::down_tile(const TileIndex& index) const
{
	if (index.z == 0) return Tiles::air;
	
	const auto pos = index.offset(0, 0, -1);

	return get_tile();
}

#undef get_tile
#undef try_get_tile