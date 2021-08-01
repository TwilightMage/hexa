#include "MeshComponent.h"

#include <OGRE/Main/OgreEntity.h>
#include <OGRE/Main/OgreMaterialManager.h>
#include <OGRE/Main/OgreSceneManager.h>
#include <OGRE/Main/OgreSceneNode.h>
#include <OGRE/Main/OgreSubEntity.h>

#include "Entity.h"
#include "Material.h"
#include "StaticMesh.h"
#include "World.h"

MeshComponent::MeshComponent()
    : mesh_(nullptr)
{
}

void MeshComponent::on_start()
{
    if (auto owner = get_owner())
    {
        if (auto world = owner->get_world())
        {
            if (mesh_)
            {
                ogre_entity_ = world->manager_->createEntity(mesh_->name.c());
                owner->scene_node_->attachObject(ogre_entity_);
            }
        }
    }
}

void MeshComponent::on_destroy()
{
    if (auto owner = get_owner())
    {
        if (auto world = owner->get_world())
        {
            if (ogre_entity_)
            {
                owner->scene_node_->detachObject(ogre_entity_);
                world->manager_->destroyEntity(ogre_entity_);
            }
        }
    }
}

void MeshComponent::set_mesh(const Shared<StaticMesh>& mesh)
{
    if (auto owner = get_owner())
    {
        mesh_ = mesh;
        
        if (auto world = owner->get_world())
        {
            if (ogre_entity_)
            {
                owner->scene_node_->detachObject(ogre_entity_);
                world->manager_->destroyEntity(ogre_entity_);
            }

            ogre_entity_ = nullptr;

            if (mesh)
            {
                ogre_entity_ = world->manager_->createEntity(mesh_->ogre_mesh_);
                owner->scene_node_->attachObject(ogre_entity_);
            }
        }
    }
}

void MeshComponent::set_material(const Shared<Material>& material, uint index)
{
    if (ogre_entity_)
    {
        if (index < ogre_entity_->getNumSubEntities())
        {
            ogre_entity_->getSubEntity(index)->setMaterial(material->ogre_material_ ? material->ogre_material_ : Ogre::MaterialManager::getSingleton().getByName("BaseWhite"));
        }
    }
}
