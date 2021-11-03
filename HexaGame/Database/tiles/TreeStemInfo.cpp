﻿#include "TreeStemInfo.h"

#include <HexaGame/HexaCollisionMaskBits.h>

#include "Engine/Material.h"
#include "Engine/MeshComponent.h"
#include "Engine/Random.h"
#include "Engine/World.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/HexaMath.h"
#include "HexaGame/ComplexTileCustomData/TreeStemCustomData.h"
#include "HexaGame/Database/Items.h"
#include "HexaGame/Database/Tiles.h"
#include "HexaGame/Entities/ComplexTile.h"
#include "HexaGame/Worlds/HexaWorld.h"

void TreeStemInfo::post_loading()
{
    ComplexTileInfo::post_loading();

    const auto basic_mat = Game::get_instance()->get_material("Hexa/Basic");
    const auto foliage_mat = Game::get_instance()->get_material("Hexa/Foliage");
    
    root_material_ = basic_mat->clone(key.to_string() + "_root");
    root_material_->set_texture(root_texture, 0);

    krone_material_ = foliage_mat->clone(key.to_string() + "_krone");
    krone_material_->set_texture(krone_texture, 0);

    branch_material_ = basic_mat->clone(key.to_string() + "_branch");
    branch_material_->set_texture(branch_texture, 0);

    branch_krone_material_ = foliage_mat->clone(key.to_string() + "_branch_krone");
    branch_krone_material_->set_texture(branch_krone_texture, 0);
}

Shared<ComplexTileCustomData> TreeStemInfo::create_custom_data() const
{
    return MakeShared<TreeStemCustomData>();
}

void TreeStemInfo::setup_spawned_entity(const Shared<ComplexTile>& new_tile, const Shared<ComplexTileCustomData>& custom_data) const
{
    if (const auto tree_custom_data = cast<TreeStemCustomData>(custom_data))
    {
        Random tree_random(tree_custom_data->tree_seed);
        
        new_tile->set_location(new_tile->get_location() + Vector3(
            tree_random.number(-0.1f, 0.1f),
            tree_random.number(-0.1f, 0.1f),
            0)
        );

        Random random(tree_custom_data->tree_seed + tree_custom_data->cell_index);
        new_tile->set_rotation(Quaternion(Vector3(0, 0, tree_random.number(360.0f) + random.number<uint>(6) * 60.0f)));

        switch (tree_custom_data->type)
        {
            case TreeStemCustomData::Type::Roots:
                {
                    const uint root_count = random.number(3, 6);
                    for (uint i = 0; i < root_count; i++)
                    {
                        auto root = MakeShared<Entity>();
                        auto root_mesh_comp = root->create_component<MeshComponent>();
                        root_mesh_comp->set_mesh(root_mesh);
                        root_mesh_comp->set_material(root_material_, 0);
                        new_tile->get_world()->spawn_entity(root, Transform(new_tile->get_location(), Quaternion(Vector3(0, 0, (360.0f / root_count * i) + random.number(-1.0f, 1.0f) * 360.0f / root_count / 2 / root_count))));

                        tree_custom_data->tree_sub_parts.add(root);
                    }
                }
                break;
            case TreeStemCustomData::Type::Top:
                {
                    const uint krone_count = random.number(3, 5);
                    for (uint i = 0; i < krone_count; i++)
                    {
                        const Vector3 point = new_tile->get_location() + Quaternion(Vector3(0, 0, random.number(360.0f))).forward() * Vector3(1, 1, 2.0f) * 0.8f;

                        auto krone = MakeShared<Entity>();
                        auto krone_mesh_comp = krone->create_component<MeshComponent>();
                        krone_mesh_comp->set_mesh(krone_mesh);
                        krone_mesh_comp->set_material(krone_material_, 0);
                        krone_mesh_comp->set_material_parameter(Quaternion(krone_mesh->get_bounds_half_size().z * 2, 1, 0, 0), 0, 0);
                        
                        new_tile->get_world()->spawn_entity(krone, Transform(point, Quaternion(Vector3(0, 0, random.number(360.0f)))));
                        const Vector2 scale = Vector2(random.number(0.8f, 1.2f), random.number(0.8f, 1.2f)) * 3.5f;
                        krone->set_scale(Vector3(scale.x, scale.x, scale.y));

                        tree_custom_data->tree_sub_parts.add(krone);
                    }
                }
                break;
            case TreeStemCustomData::Type::Branched:
                {
                    const uint branch_count = (random.boolean() ? 1 : 2) == 2 ? random.boolean() ? 2 : 3 : 1;
                    const float angle_offset = random.number(360.0f);
                    for (uint i = 0; i < branch_count; i++)
                    {
                        const float angle = angle_offset + (360.0f / branch_count * i) + random.number(-1.0f, 1.0f) * 360.0f / branch_count / 2 / branch_count;
                        const Quaternion rotation = Quaternion(Vector3(0, 0, angle));

                        const Vector3 branch_point = new_tile->get_location();

                        auto branch = MakeShared<Entity>();
                        //branch->set_mesh(branch_mesh);
                        //new_tile->get_world()->spawn_entity(branch, branch_point, rotation);
                        //branch->get_material_instance()->set_param_value("texture", branch_texture);

                        tree_custom_data->tree_sub_parts.add(branch);

                        const Vector3 krone_point = new_tile->get_location() + rotation.forward() * 0.5f + Vector3(0, 0, 0.35f);

                        auto krone = MakeShared<Entity>();
                        //krone->set_mesh(branch_krone_mesh);
                        //new_tile->get_world()->spawn_entity(krone, krone_point, rotation);
                        //krone->set_material(HexaGame::foliage_material);
                        //krone->get_material_instance()->set_param_value("texture", branch_krone_texture);
                        //krone->get_material_instance()->set_param_value("height", branch_krone_mesh->get_bounds_half_size().z * 2);

                        tree_custom_data->tree_sub_parts.add(krone);
                    }
                }
                break;
            default:;
        }
    }
}

void TreeStemInfo::on_tile_break(const TileIndex& index, const Shared<HexaWorld>& world) const
{
    world->spawn_drop(index, HexaGame::item_database->get(log_item_name), Quaternion(Vector3(0, 0, Random::static_number(360.0f))).forward() * 1.0f * Random::static_number<float>() + Vector3(0, 0, 0.2f));
}

void TreeStemInfo::on_tile_destroyed(const TileIndex& index, const Shared<ComplexTile>& destroyed_tile, const Shared<HexaWorld>& world) const
{
    if (const auto tree_custom_data = cast<TreeStemCustomData>(world->get_custom_data(index)))
    {
        for (auto& sub_part : tree_custom_data->tree_sub_parts)
        {
            sub_part->destroy();
        }

        tree_custom_data->tree_sub_parts.clear();
    }
}

void TreeStemInfo::neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, ConstPtr<TileInfo> new_neighbor) const
{
    if (new_neighbor == Tiles::air && side == TileSide::Down)
    {
        world->break_tile(index);
    }
}