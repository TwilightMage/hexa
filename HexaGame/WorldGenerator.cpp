#include "WorldGenerator.h"

#include "Engine/Math.h"

const Vector2 tex_size(60, 45);
const Vector2 floor_uv_pos = Vector2(13 / tex_size.x, 33 / tex_size.y);
const Vector2 ceil_uv_pos = Vector2(39 / tex_size.x, 33 / tex_size.y);
const Vector2 cap_mult = Vector2(13, 12) / tex_size;

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
		src_indices.Add(indices[i + 1] + offset);
		src_indices.Add(indices[i] + offset);
		src_indices.Add(indices[0] + offset);
	}
}

#define random_f()	((rand() % 101) / 100.0f)
#define random_i(n)	(rand() % (n))

void WorldGenerator::generate_tile(TileSide sides, const Shared<const TileInfo>& tileInfo, List<Mesh::vertex>& vertices, List<uint>& indices)
{
    int vertexCount = 0;

	if (!!(sides & TileSide::Up)) vertexCount += 6;
	if (!!(sides & TileSide::Down)) vertexCount += 6;
	if (!!(sides & TileSide::Front)) vertexCount += 4;
	if (!!(sides & TileSide::FrontRight)) vertexCount += 4;
	if (!!(sides & TileSide::BackRight)) vertexCount += 4;
	if (!!(sides & TileSide::Back)) vertexCount += 4;
	if (!!(sides & TileSide::BackLeft)) vertexCount += 4;
	if (!!(sides & TileSide::FrontLeft)) vertexCount += 4;

	vertices = List<Mesh::vertex>(vertexCount);
	indices.Clear();

	int offset = 0;

	// Floor
	if (!!(sides & TileSide::Down))
	{
        const int angleOffset = tileInfo->randomize_floor_uv_angle ? random_i(6) : 0;

		for (uint i = 0; i < 6; i++)
		{
			vertices[i + offset] = {
				Vector3(pos_coses[i], pos_sines[i % 6], 0.0f),
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
		const int angleOffset = tileInfo->randomize_floor_uv_angle ? random_i(6) : 0;

		for (uint i = 0; i < 6; i++)
		{
			vertices[i + offset] = {
				Vector3(pos_coses[i], pos_sines[i % 6], 1.0f / 3),
                floor_uv_pos + Vector2(uv_coses[(i + angleOffset) % 6] * cap_mult.x, uv_sines[(i + angleOffset) % 6] * cap_mult.y),
                Vector3::one()
            };
		}

		add_poly(indices, { 0, 1, 2, 3, 4, 5 }, offset);

		offset += 6;
	}

	/*// Walls
	if (sides & AnyFlatTileSide)
	{
		float yOffset = 0.0f;
		float xOffset = 0.0f;

		if (tileInfo.RandomizeWallYUVPos == ERandomiseWallUVPos::Slide)
		{
			yOffset = random_i(wallH * (wallYSteps - 1)) * pixH;
		}
		else if (tileInfo.RandomizeWallYUVPos == ERandomiseWallUVPos::Step)
		{
			yOffset = random_i(wallYSteps) * wallH * pixH;
		}

		if (tileInfo.RandomizeWallXUVPos == ERandomiseWallUVPos::Slide)
		{
			xOffset = random_i(texW) * pixW;
		}
		else if (tileInfo.RandomizeWallXUVPos == ERandomiseWallUVPos::Step)
		{
			xOffset = random_i(6) * wallW * pixW;
		}

		for (int i = 0; i <= 6; i++)
		{
			float x = FMath::Cos(FMath::DegreesToRadians(i * 60.0f - 90));
			float y = FMath::Sin(FMath::DegreesToRadians(i * 60.0f - 90));
			result.vertices[i + offset] = FVector(x * 50.0f, y * 50.0f, 0.0f); // bottom
			result.vertices[i + offset + 7] = FVector(x * 50.0f, y * 50.0f, TileHeight); // top
			result.UV[i + offset] = FVector2D(xOffset + i * WallPercentX, yOffset + WallPercentY);
			result.UV[i + offset + 7] = FVector2D(xOffset + i * WallPercentX, yOffset);
		}

		if (sides & (1 << (int)ETileSide::FrontLeft))   UMeshPieceFunctions::AddPoly(result, { 0, 1, 8,  7  }, offset); // front-right
		if (sides & (1 << (int)ETileSide::Front))       UMeshPieceFunctions::AddPoly(result, { 1, 2, 9,  8  }, offset); // front
		if (sides & (1 << (int)ETileSide::FrontRight))  UMeshPieceFunctions::AddPoly(result, { 2, 3, 10, 9  }, offset); // front-left
		if (sides & (1 << (int)ETileSide::BackRight))   UMeshPieceFunctions::AddPoly(result, { 3, 4, 11, 10 }, offset); // back-left
		if (sides & (1 << (int)ETileSide::Back))        UMeshPieceFunctions::AddPoly(result, { 4, 5, 12, 11 }, offset); // back
		if (sides & (1 << (int)ETileSide::BackLeft))    UMeshPieceFunctions::AddPoly(result, { 5, 6, 13, 12 }, offset); // back-right
	}*/
}
