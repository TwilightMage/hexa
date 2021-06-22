#include "Character.h"

#include "HexaMath.h"
#include "WorldPath.h"
#include "Worlds/HexaWorld.h"
#include "Engine/AnimatorComponent.h"

void Character::tick(float delta_time)
{
    set_rotation(Quaternion(Vector3(0, 0, Math::lerp_cycled(get_rotation().yaw(), get_desired_rotation_angle(), 0.1f, -180.0f, 180.0f))));

    if (is_in_path_segment_)
    {
        auto& path_segment = current_path_->segments[path_segment_index_];
        set_position(Math::lerp(path_segment.from.to_vector(), path_segment.to.to_vector(), move_x) + Vector3(0, 0, HexaMath::tile_height * move_y));
    }
}

void Character::go_to(const TileIndex& tile_position)
{
    if (auto world = cast<HexaWorld>(get_world()))
    {
        PathConfig config;
        config.from = get_tile_position();
        config.to = tile_position;
        config.agent_height = 2;
        if (auto path = world->FindPath(config))
        {
            new_path_to_use_ = path;
            if (!is_in_path_segment_)
            {
                start_next_path_segment();
            }
        }
    }
}

void Character::rotate_to(Direction direction)
{
    looking_direction_ = direction;
}

void Character::rotate_to_tile(const TileIndex& tile_index)
{
    looking_to_tile_ = tile_index;
    has_looking_to_tile_ = true;
}

void Character::undo_rotate_to_tile()
{
    has_looking_to_tile_ = false;
}

void Character::on_character_possesed()
{
}

void Character::on_character_un_possesed()
{
}

void Character::generate_components()
{
    animator_ = MakeShared<AnimatorComponent>();
    add_component(animator_);
}

void Character::modify_matrix_params(Vector3& position, Quaternion& rotation, Vector3& scale)
{
    position += animation_position_offset_;
}

void Character::set_tile_position(const TileIndex& tile_position)
{
    tile_position_ = tile_position;
    set_position(tile_position_.to_vector());
}

void Character::start_next_path_segment()
{
    if (!current_path_ && !new_path_to_use_) return;

    is_in_path_segment_ = true;
    
    if (new_path_to_use_)
    {
        current_path_ = new_path_to_use_;
        path_segment_index_ = 0;
        new_path_to_use_ = nullptr;
    }
    else
    {
        path_segment_index_++;
    }
    
    auto& segment = current_path_->segments[path_segment_index_];
    if (TileIndex::distance_xy(segment.from, segment.to) == 1)
    {
        if (segment.from.z > segment.to.z) // jump down
        {
            
        }
        else if (segment.from.z == segment.to.z) // step
        {
            TileSide step_direction = TileIndex::offset_to_side(segment.to - segment.from);
            rotate_to((Direction)Math::log2((float)step_direction));
            auto anim_handle = animator_->play_animation(step_animation.animation, 0, 1, step_animation.reverse);
            anim_handle.on_end()->bind(this, &Character::path_segment_finished);
        }
        else // climb
        {
            
        }
    }
}

void Character::path_segment_finished()
{
    is_in_path_segment_ = false;

    set_tile_position(current_path_->segments[path_segment_index_].to);

    if (path_segment_index_ == current_path_->segments.length() - 1)
    {
        current_path_ = nullptr;
        path_segment_index_ = 0;
        return;
    }

    start_next_path_segment();
}

float Character::get_desired_rotation_angle() const
{
    if (has_looking_to_tile_ && looking_to_tile_ != tile_position_ && !is_in_path_segment_)
    {
        auto from = looking_to_tile_.to_vector();
        auto to = tile_position_.to_vector();
        from.z = 0;
        to.z = 0;
        auto delta = (from - to);
        return Quaternion::look_at(delta.normalized()).yaw();
    }
    else
    {
        return static_cast<float>(looking_direction_) * 60;
    }
}
