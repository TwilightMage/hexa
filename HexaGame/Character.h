#pragma once

#include "Direction.h"
#include "TileIndex.h"
#include "Engine/AnimatedField.h"
#include "Engine/AnimationSlot.h"
#include "Engine/Entity.h"
#include "Engine/ITickable.h"

class Animation;
class WorldPath;
class AnimatorComponent;
class HexaWorld;
class CharacterController;

class EXPORT Character : public Entity, public ITickable, public Animated
{
    friend CharacterController;
    friend HexaWorld;

public:
    void tick(float delta_time) override;
    
    void go_to(const TileIndex& tile_position);

    void rotate_to(Direction direction);
    void rotate_to_tile(const TileIndex& tile_index);
    void undo_rotate_to_tile();
    
    virtual void on_character_possesed();
    virtual void on_character_un_possesed();

    FORCEINLINE const TileIndex& get_tile_position() const { return tile_position_; }

    FORCEINLINE const Shared<AnimatorComponent> get_animator() const { return animator_; }

protected:
    void generate_components() override;

    void modify_matrix_params(Vector3& position, Quaternion& rotation, Vector3& scale) override;

    AnimationSlot step_animation;
    
private:
    void set_tile_position(const TileIndex& tile_position);
    void start_next_path_segment();
    void path_segment_finished();

    float get_desired_rotation_angle() const;

    Vector3 animation_position_offset_;
    
    TileIndex tile_position_;
    Direction looking_direction_;
    TileIndex looking_to_tile_;
    bool has_looking_to_tile_ = false;

    Shared<WorldPath> current_path_;
    uint path_segment_index_ = 0;
    Shared<WorldPath> new_path_to_use_;
    bool is_in_path_segment_ = false;

    float move_x, move_y;
    
    uint camera_travel_distance_ = 10;

    Weak<CharacterController> controller_;

    Shared<AnimatorComponent> animator_;

public:
    ANIMATED_FIELD(float, scale_x, 1,
        [this]() -> float{ return get_scale().x; },
        [this](float val) -> void
        {
            Vector3 scale = get_scale();
            scale.x = val;
            set_scale(scale);
        }
    );
    ANIMATED_FIELD(float, scale_y, 1,
        [this]() -> float{ return get_scale().y; },
        [this](float val) -> void
        {
            Vector3 scale = get_scale();
            scale.y = val;
            set_scale(scale);
        }
    );
    ANIMATED_FIELD(float, scale_z, 1,
        [this]() -> float{ return get_scale().z; },
        [this](float val) -> void
        {
            Vector3 scale = get_scale();
            scale.z = val;
            set_scale(scale);
        }
    );
    ANIMATED_FIELD(float, offset_x, 0,
        [this]() -> float{ return animation_position_offset_.x; },
        [this](float val) -> void
        {
            animation_position_offset_.x = val;
            mark_matrix_dirty();
        }
    );
    ANIMATED_FIELD(float, offset_y, 0,
        [this]() -> float{ return animation_position_offset_.y; },
        [this](float val) -> void
        {
            animation_position_offset_.y = val;
            mark_matrix_dirty();
        }
    );
    ANIMATED_FIELD(float, offset_z, 0,
        [this]() -> float{ return animation_position_offset_.z; },
        [this](float val) -> void
        {
            animation_position_offset_.z = val;
            mark_matrix_dirty();
        }
    );
    ANIMATED_FIELD(float, move_x, 0,
        [this]() -> float{ return move_x; },
        [this](float val) -> void { move_x = val; }
    );
    ANIMATED_FIELD(float, move_y, 0,
        [this]() -> float{ return move_y; },
        [this](float val) -> void { move_y = val; }
    );
};
