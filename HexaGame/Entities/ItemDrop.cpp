#include "ItemDrop.h"

#include "Engine/Logger.h"
#include "Engine/World.h"
#include "Engine/Physics/ConcaveMeshCollision.h"
#include "Engine/Physics/RaycastResult.h"
#include "HexaGame/HexaCollisionMaskBits.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/WorldChunk.h"
#include "HexaGame/Database/items/ItemInfo.h"
#include "HexaGame/Worlds/HexaWorld.h"

ItemDrop::ItemDrop(const ItemContainer& item)
    : Entity()
    , item_(item)
{
    set_tick_enabled(true);
    
    name = item.item ? String::format("%i %s drop", item.count, item.item->name.c()) : "Empty drop";
}

void ItemDrop::on_start()
{
    fall_pos_ = get_location() + Vector3(0, 0, offset);
    make_body_kinematic();
    set_collision_mask(HexaCollisionMaskBits::ITEM);
    
    if (!item_.is_empty())
    {
        //set_mesh(item_.item->mesh);
        //get_material_instance()->set_param_value("texture", item_.item->mesh_texture);
        //set_collision(MakeShared<ConcaveMeshCollision>(item_.item->mesh));
    }
    else
    {
        print_warning("Item Drop", "Empty item dropped!");
    }
}

void ItemDrop::on_tick(float delta_time)
{
    if (!is_sleeping_ && item_.item->mass != 0.0f)
    {
        if (const auto world = get_world())
        {
            fixed_tick_accum_ += delta_time;
            while (fixed_tick_accum_ >= fixed_tick_interval_ && !is_sleeping_)
            {
                fixed_tick(world);
                fixed_tick_accum_ -= fixed_tick_interval_;
            }
            set_location(fall_pos_ + fall_force_ * fixed_tick_accum_ - Vector3(0, 0, offset));
        }
    }
}

void ItemDrop::on_destroy()
{
    if (bound_chunk_)
    {
        bound_chunk_->on_tile_change.unbind(this, &ItemDrop::chunk_tile_change);
        bound_chunk_ = nullptr;
    }
}

void ItemDrop::set_force(const Vector3& force)
{
    fall_force_ = force;
    is_sleeping_ = false;
}

void ItemDrop::fixed_tick(const Shared<World>& world)
{
    const Vector3 fall_move = fall_force_ * fixed_tick_interval_;
    if (auto hit = world->raycast(fall_pos_, fall_pos_ + fall_move, HexaCollisionMaskBits::GROUND | HexaCollisionMaskBits::COMPLEX_BLOCK))
    {
        if (Math::acos_deg(hit->normal.dot_product(Vector3::up())) < 45.0f)
        {
            fall_pos_ = hit->location;
            fall_force_ = Vector3::zero();
            is_sleeping_ = true;

            if (const auto hexa_world = cast<HexaWorld>(world))
            {
                if (bound_chunk_ = hexa_world->get_chunk(TileIndex::from_vector(fall_pos_).get_chunk()))
                {
                    bound_chunk_->on_tile_change.bind(this, &ItemDrop::chunk_tile_change);
                }
            }

            world->play_sound_3d(HexaGame::drop_sound, fall_pos_, HexaGame::get_ambient_channel());
        }
        else
        {
            const float dist = (hit->location - fall_pos_).magnitude();
            const float dist_left = fall_move.magnitude() - dist;
            const auto cross_right = hit->normal.cross_product(Vector3::up());
            const auto cross_slide = cross_right.cross_product(hit->normal);
            fall_pos_ = hit->location + cross_slide * dist_left;
            fall_force_ = cross_slide;
        }
    }
    else
    {
        fall_pos_ = fall_pos_ + fall_move;
        fall_force_ += Vector3(0, 0, -9.8f) * fixed_tick_interval_ * item_.item->mass;
        /*const float mag = fall_force_.magnitude();
        if (mag > 2)
        {
        fall_force_ *= 2 / mag;
        }*/
    }
}

void ItemDrop::chunk_tile_change(const ChunkIndex& chunk_index, const TileIndex& tile_index)
{
    const auto current_index = TileIndex::from_vector(fall_pos_).cycle_chunk();
    if (tile_index == current_index || tile_index == current_index.offset(0, 0, -1))
    {
        is_sleeping_ = false;
    }
}
