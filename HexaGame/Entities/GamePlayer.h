#pragma once

#include "Engine/IControllable.h"
#include "Engine/Player.h"
#include "Engine/SoundHandle.h"
#include "HexaGame/CharacterController.h"
#include "HexaGame/ChunkIndex.h"
#include "HexaGame/TileIndex.h"

class WorldChunk;
class WorldChunkObserver;

class EXPORT GamePlayer : public Player, public CharacterController
{
public:
    void on_start() override;
    void key_down(KeyCode key) override;
    void key_up(KeyCode key) override;
    void mouse_button_down(int button) override;
    void mouse_button_up(int button) override;
    void scroll(const Vector2& delta) override;
    void on_tick(float delta_time) override;
    
    Shared<const CharacterController> get_as_character_controller() const override;
    Shared<CharacterController> get_as_character_controller() override;
    
    Vector2 move_;

    Shared<WorldChunkObserver> observer_;
    uint load_distance_ = 1;

private:
    void spawn_chunk_loaded(const Shared<WorldChunk>& sender);
    void character_position_changed(const Shared<Character>& sender);
    void current_chunk_tile_changed(const ChunkIndex& chunk, const TileIndex& tile);
    void check_cap();

    ChunkIndex old_chunk_;
    bool use_item_mode_ = false;

    bool rotate_camera_ = false;
    float camera_distance_ = 4;
    float desired_camera_distance_ = 4;
    float camera_pivot_z_ = 0.0f;
    float desired_camera_pivot_z = 0.0f;
    Vector3 camera_position_;
    float camera_pitch_ = -45.0f;
    float camera_yaw_ = 0.0f;
    Quaternion camera_rotation_ = Quaternion(Vector3(0, 45, 0));
    SoundHandle background_music_handle_;
};
