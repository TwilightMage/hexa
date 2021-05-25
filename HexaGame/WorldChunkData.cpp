#include "WorldChunkData.h"

#include <map>


#include "Tiles.h"
#include "WorldChunkMesh.h"
#include "WorldGenerator.h"
#include "Engine/GeometryEditor.h"
#include "Engine/List.h"
#include "Engine/Mesh.h"

void WorldChunkData::validate_and_generate_metadata()
{
	for (uint x = 0; x < chunk_size; x++)
	{
		for (uint y = 0; y < chunk_size; y++)
		{
			for (uint z = 0; z < chunk_height; z++)
			{
				if (data[x][y][z] == nullptr)
				{
					data[x][y][z] = Tiles::air.get();
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

Shared<WorldChunkMesh> WorldChunkData::generate_mesh() const
{
	std::map<Shared<const TileInfo>, List<Mesh::vertex>> vertices;

	for (int z = 0; z < chunk_height; z++)
	{
		const TileType plane_metadata_down = z > 0 ? plane_metadata[z - 1] : TileType::None;
		const TileType plane_metadata_up = z < chunk_height - 1 ? plane_metadata[z + 1] : TileType::None;
		if (count_difference(plane_metadata[z] | plane_metadata_up | plane_metadata_down) >= 2)
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
							List<Mesh::vertex> tile_vertices;
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

	Shared<WorldChunkMesh> result = MakeShared<WorldChunkMesh>();

	for (auto& kvp : vertices)
	{
		result->domains.Add({MakeShared<Mesh>("World chunk mesh domain", kvp.second), kvp.first->texture});
	}

	return result;
}

TileSide WorldChunkData::get_tile_face_flags(const TileIndex& tile_index) const
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

#define get_tile()			(data[pos.x][pos.y][pos.z])
#define try_get_tile(name)	(name ? name->data[pos.x][pos.y][pos.z] : Tiles::air.get())

const Shared<const TileInfo>& WorldChunkData::front_tile(const TileIndex& index) const
{
	const auto pos = index.offset(1, 0, 0).cycle_chunk();

	return index.x < chunk_size - 1
        ? get_tile()
        : try_get_tile(front_);
}

const Shared<const TileInfo>& WorldChunkData::front_right_tile(const TileIndex& index) const
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

const Shared<const TileInfo>& WorldChunkData::back_right_tile(const TileIndex& index) const
{
	const auto pos = index.offset(0, 1, 0).cycle_chunk();

	return index.y < chunk_size - 1
        ? get_tile()
        : try_get_tile(right_);
}

const Shared<const TileInfo>& WorldChunkData::back_tile(const TileIndex& index) const
{
	const auto pos = index.offset(-1, 0, 0).cycle_chunk();

	return index.x > 0
        ? get_tile()
        : try_get_tile(back_);
}

const Shared<const TileInfo>& WorldChunkData::back_left_tile(const TileIndex& index) const
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

const Shared<const TileInfo>& WorldChunkData::front_left_tile(const TileIndex& index) const
{
	const auto pos = index.offset(0, -1, 0).cycle_chunk();

	return index.y > 0
        ? get_tile()
        : try_get_tile(left_);
}

const Shared<const TileInfo>& WorldChunkData::up_tile(const TileIndex& index) const
{
	const auto pos = index.offset(0, 0, 1);

	return get_tile();
}

const Shared<const TileInfo>& WorldChunkData::down_tile(const TileIndex& index) const
{
	const auto pos = index.offset(0, 0, -1);

	return get_tile();
}

#undef get_tile
#undef try_get_tile

void WorldChunkData::link(WorldChunkData* front, WorldChunkData* right, WorldChunkData* front_right,
	WorldChunkData* back, WorldChunkData* left, WorldChunkData* back_left)
{
	front_ = front;
	right_ = right;
	front_right_ = front_right;
	back_ = back;
	left_ = left;
	back_left_ = back_left;

	if (front_) front_->back_ = this;
	if (right_) right_->left_ = this;
	if (front_right_) front_right_->back_left_ = this;
	if (back_) back_->front_ = this;
	if (left_) left_->right_ = this;
	if (back_left_) back_left_->front_right_ = this;
}

void WorldChunkData::unlink()
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
