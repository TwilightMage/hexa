#pragma once

#include "Engine/Entity.h"
#include "Engine/IControllable.h"
#include "Engine/ITickable.h"
#include "HexaGame/CharacterController.h"
#include "HexaGame/TileIndex.h"

class WorldChunk;
class WorldChunkObserver;

class EXPORT GamePlayer : public Entity, public CharacterController, public IControllable, public ITickable
{
public:
    void on_start() override;
    void key_down(int key) override;
    void key_up(int key) override;
    void mouse_button_down(int button) override;
    void on_possess() override;
    void tick(float delta_time);
    
    Shared<const CharacterController> get_as_character_controller() const override;
    Shared<CharacterController> get_as_character_controller() override;
    
    Vector2 move_;

    Shared<Camera> camera_;

    Shared<WorldChunkObserver> observer_;
    uint load_distance_ = 2;

private:
    void spawn_chunk_loaded(const Shared<WorldChunk>& sender);
};
