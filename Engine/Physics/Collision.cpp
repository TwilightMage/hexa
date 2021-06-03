#include "Collision.h"

#include "Engine/Game.h"

Shared<reactphysics3d::PhysicsCommon> Collision::get_physics()
{
    return Game::instance_->physics_;
}
