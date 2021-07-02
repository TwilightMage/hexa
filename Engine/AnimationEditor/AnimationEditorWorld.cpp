#include "AnimationEditorWorld.h"

#include "AnimationEditorUI.h"
#include "Engine/Animated.h"
#include "Engine/AnimatorComponent.h"
#include "Engine/EditorPlayer.h"
#include "Engine/Game.h"

void AnimationEditorWorld::on_start()
{
    player_ = MakeShared<EditorPlayer>();
    player_->position_offset = Vector3(0, 1, 0);
    spawn_entity(player_, Vector3(-2, 0, 0));
    Game::possess(player_);

    ui_ = MakeShared<AnimationEditorUI>();
    Game::add_ui(ui_);
}

void AnimationEditorWorld::open(const Shared<Animated>& animated)
{
    if (auto entity = cast<Entity>(animated))
    {
        spawn_entity(entity);
        if (const auto animator = entity->find_component<AnimatorComponent>())
        {
            ui_->animated = animated;
            ui_->target = animator;
        }
        else
        {
            entity->destroy();
        }
    }
}
