#include "CameraComponent.h"

#include <OGRE/Main/OgreSceneManager.h>

#include "Entity.h"
#include "Game.h"
#include "World.h"

void CameraComponent::on_start()
{
    if (auto owner = get_owner())
    {
        if (auto world = owner->get_world())
        {
            ogre_camera_ = world->manager_->createCamera("Camera");
            ogre_camera_->setNearClipDistance(1);
            ogre_camera_->setFarClipDistance(100000);
            ogre_camera_->setAutoAspectRatio(true);
            ogre_camera_->setFOVy(Ogre::Radian(Ogre::Degree(45.0f)));
            owner->scene_node_->attachObject(ogre_camera_);
        }
    }
}

void CameraComponent::on_destroy()
{
    if (auto owner = get_owner())
    {
        if (auto world = owner->get_world())
        {
            owner->scene_node_->detachObject(ogre_camera_);
            world->manager_->destroyCamera(ogre_camera_);
        }
    }
}

bool CameraComponent::WorldToViewport(const Vector3& world, Vector2& out_screen) const
{
    const Ogre::Vector3 hcsPosition = ogre_camera_->getProjectionMatrix() * (ogre_camera_->getViewMatrix() * cast_object<Ogre::Vector3>(world));

    if ((hcsPosition.x < -1.0f) || (hcsPosition.x > 1.0f) || (hcsPosition.y < -1.0f) || (hcsPosition.y > 1.0f)) return false;

    const int nCWidth = (Game::get_instance()->get_screen_width() / 2);
    const int nCHeight = (Game::get_instance()->get_screen_height() / 2);

    out_screen.x = nCWidth + (nCWidth * -hcsPosition.x);
    out_screen.y = nCHeight + (nCHeight * hcsPosition.y);

    return true;
}

Vector3 CameraComponent::ViewportToWorld(const Vector2& screen) const
{
    const auto ray = ogre_camera_->getCameraToViewportRay(screen.x, screen.y);
    return cast_object<Vector3>(ray.getPoint(1) - ray.getPoint(0));
}
