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

void WorldGenerator::generate_tile_mesh(TileSide sides, ConstPtr<SolidTileInfo> tileInfo, List<StaticMesh::Vertex>& vertices, List<uint>& indices, float seed)
{
    int vertexCount = 0;
	uint int_seed = (uint)(seed * 127.2355f);

	if (!!(sides & TileSide::Up)) vertexCount += 6;
	if (!!(sides & TileSide::Down)) vertexCount += 6;
	if (!!(sides & TileSide::Front)) vertexCount += 4;
	if (!!(sides & TileSide::FrontRight)) vertexCount += 4;
	if (!!(sides & TileSide::BackRight)) vertexCount += 4;
	if (!!(sides & TileSide::Back)) vertexCount += 4;
	if (!!(sides & TileSide::BackLeft)) vertexCount += 4;
	if (!!(sides & TileSide::FrontLeft)) vertexCount += 4;

	if (vertexCount == 0) return;

	vertices = List<StaticMesh::Vertex>(vertexCount);
	indices.clear();

	int offset = 0;

	// Floor
	if (!!(sides & TileSide::Down))
	{
        const int angleOffset = tileInfo->randomize_floor_uv_angle ? int_seed % 6 : 0;

		for (uint i = 0; i < 6; i++)
		{
			vertices[i + offset] = {
				Vector3(pos_coses[i] * 0.5f * 100, pos_sines[i] * 0.5f * 100, 0.0f),
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
		const uint angleOffset = tileInfo->randomize_floor_uv_angle ? int_seed % 6 : 0;

		for (uint i = 0; i < 6; i++)
		{
			vertices[i + offset] = {
				Vector3(pos_coses[i] * 0.5f * 100, pos_sines[i] * 0.5f * 100, HexaMath::tile_height * 100),
                ceil_uv_pos + Vector2(uv_coses[(i + angleOffset) % 6] * cap_mult.x, uv_sines[(i + angleOffset) % 6] * cap_mult.y),
                Vector3::one()
            };
		}

		add_poly(indices, { 0, 1, 2, 3, 4, 5 }, offset);

		offset += 6;
	}

	// Walls
	static TileSide sideOrder[6] = { TileSide::FrontLeft, TileSide::Front, TileSide::FrontRight, TileSide::BackRight, TileSide::Back, TileSide::BackLeft };
	for (uint i = 0; i < 6; i++)
	{
		if (!!(sides & sideOrder[i]))
		{
			for (uint j = i; j < i + 2; j++)
			{
				uint jj = j - i;
				Vector2 pos = Vector2(pos_coses[j % 6], pos_sines[j % 6]);
				Vector2 uv_offset = Vector2((float)(int_seed % 6), (float)(int_seed % 3)) * wall_mult;

				vertices[jj + offset] = {
					Vector3(pos.x * 0.5f * 100, pos.y * 0.5f * 100, 0.0f),
					Vector2(j * wall_mult.x + uv_offset.x, wall_mult.y + uv_offset.y)
				};

				vertices[jj + offset + 2] = {
					Vector3(pos.x * 0.5f * 100, pos.y * 0.5f * 100, HexaMath::tile_height * 100),
					Vector2(j * wall_mult.x + uv_offset.x, uv_offset.y)
				};
			}

			add_poly(indices, { 0, 1, 3, 2 }, offset);

			offset += 4;
		}
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

	chunk->generate_nav_graph();
	
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
