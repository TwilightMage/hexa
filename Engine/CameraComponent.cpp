#include "CameraComponent.h"

#include <OGRE/Main/OgreSceneManager.h>

#include "Entity.h"
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
