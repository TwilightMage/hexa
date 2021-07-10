#include "WorldChunk.h"



#include "ChunkMeshEntity.h"
#include "HexaCollisionMaskBits.h"
#include "HexaGame.h"
#include "HexaSaveGame.h"
#include "Tiles.h"
#include "WorldChunkMesh.h"
#include "WorldGenerator.h"
#include "Engine/GeometryEditor.h"
#include "Engine/Material3D.h"
#include "Engine/Random.h"
#include "Engine/World.h"
#include "Engine/Physics/ConcaveMeshCollision.h"
#include "Entities/ComplexTile.h"
#include "Worlds/HexaWorld.h"

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

WorldChunk::WorldChunk(const ChunkIndex& index, const Weak<HexaWorld>& world)
    : index_(index)
    , world_(world)
    , mesh_entities_(chunk_height)
    , cap_z(chunk_height)
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

void WorldChunk::set_tile(const TileIndex& index, const Shared<const TileInfo>& new_tile)
{
    auto old_tile = data[index.x][index.y][index.z];
    
    modifications_[index] = new_tile;
    data[index.x][index.y][index.z] = new_tile;
    dirty_ = true;

    generate_metadata(index.z);

    if (auto world = world_.lock())
    {
        for (byte b = 0; b < 8; b++)
        {
            auto side = (TileSide)(1 << b);
            TileIndex neighbor = index + TileIndex::offset_from_side(side);
            if (neighbor.is_inside_chunk())
            {
                data[neighbor.x][neighbor.y][neighbor.z]->neighbor_changed(neighbor, tile_side_opposite(side), world, new_tile);
            }
        }
    }
    
    on_tile_change(index_, index);

    if (auto complex = complex_tiles_.find(index))
    {
        complex->entity->destroy();
    }
    if (new_tile->type == TileType::Complex)
    {
        auto& slot = complex_tiles_[index];
        spawn_complex(index, slot);
        
        if ((uint)index.z > cap_z)
        {
            slot.entity->set_visibility(false);
        }
    }

    if (old_tile->type == TileType::Complex && new_tile->type != TileType::Complex)
    {
        complex_tiles_.remove(index);
    }
    
    if (count_difference(new_tile->type | old_tile->type) >= 2)
    {
        if (index.z > 0 && count_difference(new_tile->type | data[index.x][index.y][index.z - 1]->type) >= 2)
        {
            regenerate_mesh(index.z - 1);
        }
        regenerate_mesh(index.z);
        if (index.z < chunk_height - 1 && count_difference(new_tile->type | data[index.x][index.y][index.z + 1]->type) >= 2)
        {
            regenerate_mesh(index.z + 1);
        }
    
        if (index.z == cap_z - 1)
        {
            regenerate_cap_mesh();
        }
    }
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
        regenerate_whole_mesh();
        regenerate_all_complex_tiles();
        regenerate_cap_mesh();
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

#define bind_tile_change(side)\
side->on_tile_change.bind(this, &WorldChunk::neighbor_tile_changed);\
this->on_tile_change.bind(side, &WorldChunk::neighbor_tile_changed);
void WorldChunk::link(WorldChunk* front, WorldChunk* right, WorldChunk* front_right,
    WorldChunk* back, WorldChunk* left, WorldChunk* back_left)
{
    if (front) front_ = front;
    if (right) right_ = right;
    if (front_right) front_right_ = front_right;
    if (back) back_ = back;
    if (left) left_ = left;
    if (back_left) back_left_ = back_left;

    if (front_)
    {
        front_->back_ = this;
        bind_tile_change(front_);
    }
    if (right_)
    {
        right_->left_ = this;
        bind_tile_change(right_);
    }
    if (front_right_)
    {
        front_right_->back_left_ = this;
        bind_tile_change(front_right_);
    }
    if (back_)
    {
        back_->front_ = this;
        bind_tile_change(back_);
    }
    if (left_)
    {
        left_->right_ = this;
        bind_tile_change(left_);
    }
    if (back_left_)
    {
        back_left_->front_right_ = this;
        bind_tile_change(back_left_);
    }
}

#define unbind_tile_change(side)\
side->on_tile_change.unbind(this, &WorldChunk::neighbor_tile_changed);\
this->on_tile_change.unbind(side, &WorldChunk::neighbor_tile_changed);
void WorldChunk::unlink()
{
    if (front_)
    {
        unbind_tile_change(front_);
        front_->back_ = nullptr;
    }
    if (right_)
    {
        unbind_tile_change(right_);
        right_->left_ = nullptr;
    }
    if (front_right_)
    {
        unbind_tile_change(front_right_);
        front_right_->back_left_ = nullptr;
    }
    if (back_)
    {
        unbind_tile_change(back_);
        back_->front_ = nullptr;
    }
    if (left_)
    {
        unbind_tile_change(left_);
        left_->right_ = nullptr;
    }
    if (back_left_)
    {
        unbind_tile_change(back_left_);
        back_left_->front_right_ = nullptr;
    }

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
        for (auto& layer_mesh : mesh_entities_)
        {
            for (auto& type_mesh_destroy : layer_mesh)
            {
                type_mesh_destroy->destroy();
            }

           layer_mesh.clear();
        }

        if (cap_entity_)
        {
            cap_entity_->destroy();
            cap_entity_ = nullptr;
        }

        for (auto& complex : complex_tiles_)
        {
            complex->value.entity->destroy();
        }
        complex_tiles_.clear();
    }
}

void WorldChunk::generate_metadata()
{
    for (uint z = 0; z < chunk_height; z++)
    {
        generate_metadata(z);
    }
}

void WorldChunk::generate_metadata(uint z)
{
    plane_metadata[z] = TileType::None;
    
    for (uint x = 0; x < chunk_size; x++)
    {
        for (uint y = 0; y < chunk_size; y++)
        {
            if (data[x][y][z] == nullptr)
            {
                data[x][y][z] = Tiles::air;
            }
				
            plane_metadata[z] |= data[x][y][z]->type;
        }
    }
}

void WorldChunk::neighbor_tile_changed(const ChunkIndex& chunk, const TileIndex& index)
{
    if (auto world = world_.lock())
    {
        List<TileSide> sender_sides;
        if (chunk == index_ + ChunkIndex(1, 0)) // front
        {
            if (index.x == 0)
            {
                sender_sides = { TileSide::Back };
                if (index.y > 0) sender_sides.add(TileSide::BackLeft);
            }
        }
        else if (chunk == index_ + ChunkIndex(1, 1)) // front right
        {
            if (index.x == 0 && index.y == 0)
            {
                sender_sides = { TileSide::BackLeft};
            }
        }
        else if (chunk == index_ + ChunkIndex(0, 1)) // right
        {
            if (index.y == 0)
            {
                sender_sides = { TileSide::FrontLeft };
                if (index.x > 0) sender_sides.add(TileSide::BackLeft);
            }
        }
        else if (chunk == index_ - ChunkIndex(1, 0)) // back
        {
            if (index.x == chunk_size - 1)
            {
                sender_sides = { TileSide::Front };
                if (index.y < chunk_size - 1) sender_sides.add(TileSide::FrontRight);
            }
        }
        else if (chunk == index_ - ChunkIndex(1, 1)) // back left
        {
            if (index.x == chunk_size - 1 && index.y == chunk_size - 1)
            {
                sender_sides = { TileSide::FrontRight };
            }
        }
        else if (chunk == index_ - ChunkIndex(0, 1)) // left
        {
            if (index.y == chunk_size - 1)
            {
                sender_sides = { TileSide::FrontRight };
                if (index.x < chunk_size - 1) sender_sides.add(TileSide::BackRight);
            }
        }

        bool regenerate = false;
        auto absolute_index = index.to_absolute(chunk);
        auto new_tile = world->get_tile_id(absolute_index);
        for (auto sender_side : sender_sides)
        {
            auto neighbor = absolute_index + TileIndex::offset_from_side(sender_side);
            auto neighbor_local = neighbor.cycle_chunk();
            data[neighbor_local.x][neighbor_local.y][neighbor_local.z]->neighbor_changed(neighbor, tile_side_opposite(sender_side), world, new_tile);
            regenerate |= count_difference(data[neighbor_local.x][neighbor_local.y][neighbor_local.z]->type | new_tile->type) >= 2;
        }

        if (regenerate)
        {
            regenerate_mesh(index.z);
        }
    }
}

void WorldChunk::spawn_complex(const TileIndex& local_index, ComplexTileSlot& slot) const
{
    const TileIndex world_index = local_index.to_absolute(index_);
    if (auto world = world_.lock())
    {
        if (slot.entity)
        {
            slot.entity->destroy();
        }
        
        auto entity = MakeShared<ComplexTile>(slot.info);
        entity->index_ = world_index;
        world->spawn_entity(entity, world_index.to_vector());

        entity->tile_info_->setup_spawned_entity(entity, custom_data_.find_or_default(local_index, nullptr));

        slot.entity = entity;
    }
}

bool WorldChunk::can_claim(const TileIndex& local_index) const
{
    return complex_tiles_.contains(local_index.cycle_chunk());
}

bool WorldChunk::claim_tile(const TileIndex& local_index, const Shared<ComplexTile>& claimer)
{
    auto& slot = complex_tiles_[local_index];

    if (slot.entity == nullptr)
    {
        slot = { nullptr, claimer };
        return true;
    }

    return false;
}

void WorldChunk::regenerate_all_complex_tiles()
{
    if (auto world = world_.lock())
    {
        for (auto& complex : complex_tiles_)
        {
            spawn_complex(complex->key, complex->value);
        }
    }
}

void WorldChunk::regenerate_whole_mesh(bool fill_complex)
{
    for (uint i = 0; i < chunk_height; i++)
    {
        regenerate_mesh(i, true);
    }
}

void WorldChunk::regenerate_mesh(uint z, bool fill_complex)
{
    for (auto& type_mesh : mesh_entities_[z])
    {
        type_mesh->destroy();
    }
    
    mesh_entities_[z].clear();

    if (fill_complex)
    {
        bool z_met = false;
        for (uint i = 0; i < complex_tiles_.entries.length(); i++)
        {
            if (complex_tiles_.entries[i]->key.z == z)
            {
                z_met = true;
                complex_tiles_.entries.remove_at(i--);
            }
            else if (z_met)
            {
                break;
            }
        }
    }
    
    if (auto world = world_.lock())
    {
        std::map<Shared<const SolidTileInfo>, List<Mesh::Vertex>> type_vertices;

        const TileType plane_metadata_front_right = front_right_->plane_metadata[z];
        const TileType plane_metadata_back_left = back_left_->plane_metadata[z];
        const TileType plane_metadata_right = right_->plane_metadata[z];
        const TileType plane_metadata_left = left_->plane_metadata[z];
        const TileType plane_metadata_forward = front_->plane_metadata[z];
        const TileType plane_metadata_back = back_->plane_metadata[z];
        const TileType plane_metadata_down = z > 0 ? plane_metadata[z - 1] : TileType::Air;
        const TileType plane_metadata_up = z < chunk_height - 1 ? plane_metadata[z + 1] : TileType::Air;

        const bool build_tiles = count_difference(plane_metadata[z] | plane_metadata_up | plane_metadata_down | plane_metadata_forward | plane_metadata_back | plane_metadata_right | plane_metadata_left | plane_metadata_front_right | plane_metadata_back_left) >= 2;
        
        if (build_tiles || fill_complex)
        {
            for (int x = 0; x < chunk_size; x++)
            {
                for (int y = 0; y < chunk_size; y++)
                {
                    const auto index = TileIndex(x, y, z);
                    const auto tile = data[x][y][z];
                    
                    if (build_tiles)
                    {
                        if (const auto& solid_tile = cast<SolidTileInfo>(tile))
                        {
                            TileSide side_flags = get_tile_face_flags(index);

                            if (side_flags != TileSide::None)
                            {
                                Vector3 world_pos = index.to_vector();
                                List<Mesh::Vertex> tile_vertices;
                                List<uint> tile_indices;
                                WorldGenerator::generate_tile_mesh(side_flags, solid_tile, tile_vertices, tile_indices, world_pos.sum_all());

                                GeometryEditor::remove_indices(tile_vertices, tile_indices);
                                GeometryEditor::translate(tile_vertices, world_pos);

                                type_vertices[solid_tile] += tile_vertices;
                            }
                        }
                    }

                    if (fill_complex)
                    {
                        if (const auto& complex_tile = cast<ComplexTileInfo>(tile))
                        {
                            complex_tiles_[index].info = complex_tile;
                        }
                    }
                }
            }
        }

        for (auto& kvp : type_vertices)
        {
            auto mesh = MakeShared<Mesh>(String::format("Chunk {%i %i %i} %s", index_.x, index_.y, z, kvp.first->key.c()), kvp.second);
            
            auto mesh_entity = MakeShared<ChunkMeshEntity>();
            mesh_entity->set_mesh(mesh);
            mesh_entity->get_material_instance()->set_param_value("texture", kvp.first->texture);
            world->spawn_entity(mesh_entity, index_.to_vector());

            mesh_entity->set_collision(MakeShared<ConcaveMeshCollision>(mesh));

            mesh_entity->set_visibility(z < cap_z);
        
            mesh_entities_[z].add(mesh_entity);
        }
    }
}

void WorldChunk::regenerate_cap_mesh()
{
    if (cap_entity_)
    {
        cap_entity_->destroy();
        cap_entity_ = nullptr;
    }
    
    if (auto world = world_.lock())
    {
        List<Mesh::Vertex> cap_vertices;
        for (int x = 0; x < chunk_size; x++)
        {
            for (int y = 0; y < chunk_size; y++)
            {
                const auto index = TileIndex(x, y, cap_z - 1);
                const auto& tile = cast<SolidTileInfo>(data[x][y][cap_z - 1]);

                if (tile)
                {
                    TileSide side_flags = get_tile_face_flags(index);

                    if (!!(~side_flags & TileSide::Up))
                    {
                        Vector3 world_pos = index.to_vector();
                        List<Mesh::Vertex> tile_vertices;
                        List<uint> tile_indices;
                        const float sa = world_pos.sum_all();
                        WorldGenerator::generate_tile_mesh(TileSide::Up, tile, tile_vertices, tile_indices, sa);

                        const Vector2 offset = Vector2(sa * 3, sa);
                        for (auto& v : tile_vertices)
                        {
                            v.uv += offset;
                        }

                        GeometryEditor::remove_indices(tile_vertices, tile_indices);
                        GeometryEditor::translate(tile_vertices, world_pos);

                        cap_vertices += tile_vertices;
                    }
                }
            }
        }

        if (cap_vertices.length() > 0)
        {
            auto mesh = MakeShared<Mesh>(String::format("Chunk {%i %i %i} cap", index_.x, index_.y, cap_z - 1), cap_vertices);
            
            cap_entity_ = MakeShared<ChunkMeshEntity>();
            cap_entity_->set_material(HexaGame::tile_cap_material);
            cap_entity_->set_mesh(mesh);
            world->spawn_entity(cap_entity_, index_.to_vector());

            cap_entity_->set_collision(MakeShared<ConcaveMeshCollision>(mesh));
        }
    }
}

void WorldChunk::cap(uint z)
{
    if (z == cap_z) return;

    cap_z = z;
    
    for (uint i = 0; i < mesh_entities_.length(); i++)
    {
        for (auto& type_entity : mesh_entities_[i])
        {
            type_entity->set_visibility(i < z);
        }
    }

    if (visibility_counter_ > 0)
    {
        regenerate_cap_mesh();

        for (auto& complex : complex_tiles_)
        {
            complex->value.entity->set_visibility((uint)complex->key.z <= z);
        }
    }
}

#define get_tile()			(data[pos.x][pos.y][pos.z])
#define try_get_tile(name)	(name ? name->data[pos.x][pos.y][pos.z] : Tiles::air)

Shared<const TileInfo> WorldChunk::front_tile(const TileIndex& index) const
{
	const auto pos = index.offset(1, 0, 0).cycle_chunk();

	return index.x < chunk_size - 1
        ? get_tile()
        : try_get_tile(front_);
}

Shared<const TileInfo> WorldChunk::front_right_tile(const TileIndex& index) const
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

Shared<const TileInfo> WorldChunk::back_right_tile(const TileIndex& index) const
{
	const auto pos = index.offset(0, 1, 0).cycle_chunk();

	return index.y < chunk_size - 1
        ? get_tile()
        : try_get_tile(right_);
}

Shared<const TileInfo> WorldChunk::back_tile(const TileIndex& index) const
{
	const auto pos = index.offset(-1, 0, 0).cycle_chunk();

	return index.x > 0
        ? get_tile()
        : try_get_tile(back_);
}

Shared<const TileInfo> WorldChunk::back_left_tile(const TileIndex& index) const
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

Shared<const TileInfo> WorldChunk::front_left_tile(const TileIndex& index) const
{
	const auto pos = index.offset(0, -1, 0).cycle_chunk();

	return index.y > 0
        ? get_tile()
        : try_get_tile(left_);
}

Shared<const TileInfo> WorldChunk::up_tile(const TileIndex& index) const
{
	if (index.z == chunk_height - 1) return Tiles::air;
	
	const auto pos = index.offset(0, 0, 1);

	return get_tile();
}

Shared<const TileInfo> WorldChunk::down_tile(const TileIndex& index) const
{
	if (index.z == 0) return Tiles::air;
	
	const auto pos = index.offset(0, 0, -1);

	return get_tile();
}

#undef get_tile
#undef try_get_tile