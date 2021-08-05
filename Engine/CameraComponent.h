#pragma once

#include "EntityComponent.h"

class Game;

namespace Ogre
{
    class Camera;
}

class EXPORT CameraComponent : public EntityComponent
{
    friend Game;

public:
    void on_start() override;
    void on_destroy() override;

private:
    Ogre::Camera* ogre_camera_ = nullptr;
};
