#include "TreeStemInfo.h"

#include "Engine/Random.h"
#include "Engine/World.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/ComplexTileCustomData/TreeStemCustomData.h"
#include "HexaGame/Entities/ComplexTile.h"

Shared<ComplexTileCustomData> TreeStemInfo::create_custom_data() const
{
    return MakeShared<TreeStemCustomData>();
}

void TreeStemInfo::setup_spawned_entity(const Shared<ComplexTile>& new_tile, const Shared<ComplexTileCustomData>& custom_data) const
{
    if (const auto tree_custom_data = cast<TreeStemCustomData>(custom_data))
    {
        new_tile->set_position(new_tile->get_position() + Vector3(
            Random::static_number((tree_custom_data->tree_seed - 1) * RANDOM_STEP, -0.1f, 0.1f),
            Random::static_number((tree_custom_data->tree_seed + 1) * RANDOM_STEP, -0.1f, 0.1f),
            0)
        );
        new_tile->set_rotation(Quaternion(Vector3(0, 0, Random::static_number(tree_custom_data->tree_seed * RANDOM_STEP, 360.0f) + Random::static_number<uint>(tree_custom_data->cell_index * RANDOM_STEP, 6) * 60.0f)));

        switch (tree_custom_data->type)
        {
            case TreeStemCustomData::Type::Roots:
                {
                    Random random(tree_custom_data->tree_seed);

                    const uint root_count = random.number(2, 7);
                    for (uint i = 0; i < root_count; i++)
                    {
                        auto root = MakeShared<Entity>();
                        root->set_mesh(roots_meshes[random.number(roots_meshes.length())]);
                        new_tile->get_world()->spawn_entity(root, new_tile->get_position(), Quaternion(Vector3(0, 0, random.number(360.0f))));
                        root->set_material(Game::get_basic_material_3d());
                        root->get_material_instance()->set_param_value("texture", roots_textures[random.number(roots_textures.length())]);
                    }
                }
                break;
            case TreeStemCustomData::Type::Top:
                {
                    Random random(tree_custom_data->tree_seed);
                    auto krone = MakeShared<Entity>();
                    krone->set_mesh(krone_mesh);
                    new_tile->get_world()->spawn_entity(krone, new_tile->get_position(), Quaternion(Vector3(0, 0, random.number(360.0f))));
                    const float z_scale = random.number(1.2f, 1.5f);
                    krone->set_scale(Vector3(1, 1, z_scale));
                    krone->set_material(HexaGame::foliage_material);
                    krone->get_material_instance()->set_param_value("texture", krone_texture);
                    krone->get_material_instance()->set_param_value("height", krone_mesh->get_bounds_half_size().z * 2);

                    krone = MakeShared<Entity>();
                    krone->set_mesh(krone_mesh);
                    new_tile->get_world()->spawn_entity(krone, new_tile->get_position() - Vector3(0, 0, z_scale * 0.7f), Quaternion(Vector3(0, 0, random.number(360.0f))));
                    krone->set_scale(Vector3(0.6f, 0.6f, 0.8f));
                    krone->set_material(HexaGame::foliage_material);
                    krone->get_material_instance()->set_param_value("texture", krone_texture);
                    krone->get_material_instance()->set_param_value("height", krone_mesh->get_bounds_half_size().z * 2);

                    const uint sub_krones_count = random.number(7, 12);
                    for (uint i = 0; i < sub_krones_count; i++)
                    {
                        const Vector3 point = new_tile->get_position() + Quaternion(Vector3(0, random.number(-45.0f, 45.0f), random.number(360.0f))).forward() * Vector3(1, 1, z_scale) * 0.8f;

                        auto sub_krone = MakeShared<Entity>();
                        sub_krone->set_mesh(krone_mesh);
                        new_tile->get_world()->spawn_entity(sub_krone, point, Quaternion(Vector3(0, 0, random.number(360.0f))));
                        sub_krone->set_scale(Vector3(random.number(0.5f, 0.7f), random.number(0.5f, 0.7f), random.number(0.6f, 0.8f)));
                        sub_krone->set_material(HexaGame::foliage_material);
                        sub_krone->get_material_instance()->set_param_value("texture", krone_texture);
                        sub_krone->get_material_instance()->set_param_value("height", krone_mesh->get_bounds_half_size().z * 2);
                    }
                }
                break;
            case TreeStemCustomData::Type::Branched:

                break;
            default:;
        }
    }
}
