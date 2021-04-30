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

Shared<Mesh> Mesh::load_obj(const Path& path)
{
    if (path.Exists())
    {
        Shared<Mesh> result = MakeShared<Mesh>();

        List<Vec3> positions;
        List<Vec2> uvs;
        List<Triangle> triangles;

        auto reader = File::Reader::Open(path);
        bool gotObject = false;
        while (!reader->IsEndOfFile())
        {
            auto line = reader->ReadLine();
            if (!line.IsEmpty())
            {
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
                    for (uint j = 1; j < parts.Length(); j++)
                    {
                        auto triangle_strings = parts[j].Split('/');
                        triangles.Add(Triangle { StringParse<int>(triangle_strings[0]), StringParse<int>(triangle_strings[1]), StringParse<int>(triangle_strings[2]) });
                    }
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
            result->vertices.Add(vertex {
                positions[i].x, positions[i].y, positions[i].z,
                uvs[i].x, uvs[i].y,
                1.0f, 1.0f, 1.0f
            });
        }

        Game::instance_->meshes_.Add(Shared<Mesh>(result));

        return result;
    }

    return nullptr;
}

uint Mesh::get_usage_count() const
{
    return usage_count_;
}
