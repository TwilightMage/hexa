#include "WorldGenerator.h"

#include "HexaMath.h"
#include "HexaSaveGame.h"
#include "HexaSettings.h"
#include "WorldChunk.h"
#include "Engine/Game.h"
#include "Engine/Math.h"
#include "Engine/performance.h"

const Vector2 tex_size(60, 45);
const Vector2 floor_uv_pos = Vector2(39, 33) / tex_size;
const Vector2 ceil_uv_pos = Vector2(13, 33) / tex_size;
const Vector2 cap_mult = Vector2(13, 12) / tex_size;
const Vector2 wall_mult = Vector2(10, 7) / tex_size;

const float pos_coses[6] = {
	Math::cos_deg(0 * 60.0f - 90),
	Math::cos_deg(1 * 60.0f - 90),
	Math::cos_deg(2 * 60.0f - 90),
	Math::cos_deg(3 * 60.0f - 90),
	Math::cos_deg(4 * 60.0f - 90),
	Math::cos_deg(5 * 60.0f - 90)
};

const float pos_sines[6] = {
	Math::sin_deg(0 * 60.0f - 90),
    Math::sin_deg(1 * 60.0f - 90),
    Math::sin_deg(2 * 60.0f - 90),
    Math::sin_deg(3 * 60.0f - 90),
    Math::sin_deg(4 * 60.0f - 90),
    Math::sin_deg(5 * 60.0f - 90)
};

const float uv_coses[6] = {
	Math::cos_deg(0 * 60.0f),
    Math::cos_deg(1 * 60.0f),
    Math::cos_deg(2 * 60.0f),
    Math::cos_deg(3 * 60.0f),
    Math::cos_deg(4 * 60.0f),
    Math::cos_deg(5 * 60.0f)
};

const float uv_sines[6] = {
	Math::sin_deg(0 * 60.0f),
    Math::sin_deg(1 * 60.0f),
    Math::sin_deg(2 * 60.0f),
    Math::sin_deg(3 * 60.0f),
    Math::sin_deg(4 * 60.0f),
    Math::sin_deg(5 * 60.0f)
};

void add_poly(List<uint>& src_indices, List<uint> indices, int offset)
{
	for (uint i = 1; i < indices.length() - 1; i++)
	{
		src_indices.add(indices[0] + offset);
		src_indices.add(indices[i] + offset);
		src_indices.add(indices[i + 1] + offset);	
	}
}

WorldGenerator::~WorldGenerator()
{
	stop = true;
	for (auto& thread : thread_pool_)
	{
		thread->join();
	}
}

void WorldGenerator::generate_tile_mesh(TileSide sides, const Shared<const SolidTileInfo>& tileInfo, List<Mesh::Vertex>& vertices, List<uint>& indices, float seed)
{
    int vertexCount = 0;
	int int_seed = static_cast<int>(seed * 127.2355f);

	if (!!(sides & TileSide::Up)) vertexCount += 6;
	if (!!(sides & TileSide::Down)) vertexCount += 6;
	if (!!(sides & TileSide::Wall)) vertexCount += 14;

	vertices = List<Mesh::Vertex>(vertexCount);
	indices.clear();

	int offset = 0;

	// Floor
	if (!!(sides & TileSide::Down))
	{
        const int angleOffset = tileInfo->randomize_floor_uv_angle ? int_seed % 6 : 0;

		for (uint i = 0; i < 6; i++)
		{
			vertices[i + offset] = {
				Vector3(pos_coses[i] * 0.5f, pos_sines[i % 6] * 0.5f, 0.0f),
                floor_uv_pos + Vector2(uv_coses[(i + angleOffset) % 6] * cap_mult.x, uv_sines[(i + angleOffset) % 6] * cap_mult.y),
                Vector3::one()
            };
		}

		add_poly(indices, { 0, 5, 4, 3, 2, 1 }, offset);

		offset += 6;
	}

	// Ceil
	if (!!(sides & TileSide::Up))
	{
		const int angleOffset = tileInfo->randomize_floor_uv_angle ? int_seed % 6 : 0;

		for (uint i = 0; i < 6; i++)
		{
			vertices[i + offset] = {
				Vector3(pos_coses[i] * 0.5f, pos_sines[i % 6] * 0.5f, HexaMath::tile_height),
                ceil_uv_pos + Vector2(uv_coses[(i + angleOffset) % 6] * cap_mult.x, uv_sines[(i + angleOffset) % 6] * cap_mult.y),
                Vector3::one()
            };
		}

		add_poly(indices, { 0, 1, 2, 3, 4, 5 }, offset);

		offset += 6;
	}

	// Walls
	if (!!(sides & TileSide::Wall))
	{
		for (int i = 0; i < 7; i++)
		{
			const auto pos = Vector2(pos_coses[i % 6], pos_sines[i % 6]);
			const auto uv_offset = Vector2(static_cast<float>(int_seed % 6), static_cast<float>(int_seed % 3)) * wall_mult;
			vertices[i + offset] = {
				Vector3(pos.x * 0.5f, pos.y * 0.5f, 0.0f),
				Vector2(i * wall_mult.x + uv_offset.x, wall_mult.y + uv_offset.y),
				Vector3::one()
			};
			vertices[i + offset + 7] = {
				Vector3(pos.x * 0.5f, pos.y * 0.5f, HexaMath::tile_height),
                Vector2(i * wall_mult.x + uv_offset.x, uv_offset.y),
                Vector3::one()
            };
		}

		if (!!(sides & TileSide::FrontLeft))   add_poly(indices, { 0, 1, 8,  7  }, offset); // front-right
		if (!!(sides & TileSide::Front))       add_poly(indices, { 1, 2, 9,  8  }, offset); // front
		if (!!(sides & TileSide::FrontRight))  add_poly(indices, { 2, 3, 10, 9  }, offset); // front-left
		if (!!(sides & TileSide::BackRight))   add_poly(indices, { 3, 4, 11, 10 }, offset); // back-left
		if (!!(sides & TileSide::Back))        add_poly(indices, { 4, 5, 12, 11 }, offset); // back
		if (!!(sides & TileSide::BackLeft))    add_poly(indices, { 5, 6, 13, 12 }, offset); // back-right
	}
}

void WorldGenerator::request_chunk_generation(const Shared<WorldChunk>& chunk)
{
	pending_chunks_mutex_.lock();
	bool abort = false;
	for (auto& pending_chunk : pending_chunks_)
	{
		if (pending_chunk.chunk == chunk)
		{
			abort = true;
			break;
		}
	}
	
	if (abort)
	{
		pending_chunks_mutex_.unlock();
		return;
	}
	pending_chunks_.add({chunk, false});
	pending_chunks_mutex_.unlock();
}

JSON WorldGenerator::write_settings() const
{
	return JSON();
}

void WorldGenerator::read_settings(const JSON& settings)
{
}

void WorldGenerator::allocate_thread_pool()
{
	auto q = std::thread::hardware_concurrency();
	if (thread_pool_.length() == 0)
	{
		for (uint i = 0; i < std::thread::hardware_concurrency(); i++)
		{
			thread_pool_.add(MakeShared<std::thread>(&WorldGenerator::thread_loop, this));
		}
	}
}

void WorldGenerator::do_generate(const Shared<WorldChunk>& chunk)
{
	chunk->set_state(WorldChunkDataState::Loading);

	generate_chunk(EditableChunk(chunk));
	if (const auto save_game = cast<HexaSaveGame>(Game::get_save_game()))
	{
		if (const auto modifications = save_game->get_chunk_modifications(chunk->get_index()))
		{
			for (auto& modification : *modifications)
			{
				chunk->data[modification.key.x][modification.key.y][modification.key.z] = modification.value;
			}
			chunk->modifications_ = *modifications;
		}
	}
	chunk->generate_metadata();
	
	Game::call_on_main_thread([this, chunk]()
	{
		finish_generation(chunk);
	});
}

void WorldGenerator::finish_generation(const Shared<WorldChunk>& chunk)
{
	pending_chunks_mutex_.lock();
	for (uint i = 0; i < pending_chunks_.length(); i++)
	{
		if (pending_chunks_[i].chunk == chunk)
		{
			pending_chunks_.remove_at(i);
			break;
		}
	}
	pending_chunks_mutex_.unlock();

	chunk->set_state(WorldChunkDataState::Loaded);
}

void WorldGenerator::thread_loop()
{
	while (!stop)
	{
		Shared<WorldChunk> chunk_to_generate = nullptr;
		pending_chunks_mutex_.lock();
		for (auto& pending_chunk : pending_chunks_)
		{
			if (!pending_chunk.is_taken)
			{
				chunk_to_generate = pending_chunk.chunk;
				pending_chunk.is_taken = true;
				break;
			}
		}
		pending_chunks_mutex_.unlock();

		if (chunk_to_generate)
		{
			do_generate(chunk_to_generate);
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
