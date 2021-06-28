#pragma once

#include "Engine/IControllable.h"
#include "Engine/ITickable.h"
#include "Engine/Player.h"
#include "HexaGame/CharacterController.h"
#include "HexaGame/ChunkIndex.h"

class WorldChunk;
class WorldChunkObserver;

class EXPORT GamePlayer : public Player, public CharacterController, public ITickable
{
public:
    void on_start() override;
    void key_down(int key) override;
    void key_up(int key) override;
    void mouse_button_down(int button) override;
    void tick(float delta_time) override;
    
    Shared<const CharacterController> get_as_character_controller() const override;
    Shared<CharacterController> get_as_character_controller() override;
    
    Vector2 move_;

    Shared<WorldChunkObserver> observer_;
    uint load_distance_ = 3;

private:
    void spawn_chunk_loaded(const Shared<WorldChunk>& sender);
    void character_position_changed(const Shared<Character>& sender);

    ChunkIndex old_chunk_;
    bool use_item_mode_ = false;
};
