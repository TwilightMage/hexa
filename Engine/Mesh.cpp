#include "Mesh.h"

#include "File.h"
#include "Game.h"

struct Triangle
{
    int a, b, c;
};

struct Vec3
{
    float x, y, z;
};

struct Vec2
{
    float x, y;
};

Mesh* Mesh::load_obj(const Path& path)
{
    if (path.Exists())
    {
        Mesh* result = new Mesh();

        List<Vec3> positions;
        List<Vec2> uvs;
        List<Triangle> triangles;

        auto reader = File::Reader::Open(path);
        bool gotObject = false;
        while (!reader->IsEndOfFile())
        {
            auto line = reader->ReadLine();
            auto parts = line.Split(' ');
            if (parts[0] == "o")
            {
                if (gotObject)
                {
                    print_error("Mesh", "Unable to load mesh %s, cannot load more than 1 object", path.get_absolute().ToString());
                    return nullptr;
                }
                gotObject = true;
            }
            else if (parts[0] == "v")
            {
                positions.Add(Vec3 { StringParse<float>(parts[1]), StringParse<float>(parts[2]), StringParse<float>(parts[3]) });
            }
            else if (parts[0] == "vt")
            {
                uvs.Add(Vec2 { StringParse<float>(parts[1]), StringParse<float>(parts[2]) });
            }
            else if (parts[0] == "f")
            {
                for (const auto part : parts)
                {
                    auto triangle_strings = part.Split('/');
                    triangles.Add(Triangle { StringParse<int>(triangle_strings[0]), StringParse<int>(triangle_strings[1]), StringParse<int>(triangle_strings[2]) });
                }
            }
        }

        if (positions.Length() != uvs.Length())
        {
            print_error("Mesh", "Unable to load mesh %s, vertex and uv count mismatch", path.get_absolute().ToString());
            return nullptr;
        }

        for (uint i = 0; i < positions.Length(); i++)
        {
            result->vertices.Add(Vertex {
                positions[i].x, positions[i].y, positions[i].z,
                uvs[i].x, uvs[i].y,
                1.0f, 1.0f, 1.0f
            });
        }

        return result;
    }

    return nullptr;
}
