﻿#include "MeshComponent.h"

#include <OGRE/Main/OgreEntity.h>
#include <OGRE/Main/OgreMaterialManager.h>
#include <OGRE/Main/OgreSceneManager.h>
#include <OGRE/Main/OgreSceneNode.h>
#include <OGRE/Main/OgreSubEntity.h>
#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/collision/Collider.h>
#include <reactphysics3d/engine/PhysicsWorld.h>

#include "Entity.h"
#include "Game.h"
#include "Material.h"
#include "StaticMesh.h"
#include "World.h"
#include "Physics/Collision.h"

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
            rigid_body_ = world->physics_world_->createRigidBody(reactphysics3d::Transform(cast_object<reactphysics3d::Vector3>(owner->get_location()), cast_object<reactphysics3d::Quaternion>(owner->get_rotation())));
            rigid_body_->setUserData(owner.get());
            
            setup_new_mesh(mesh_);
            rigid_body_->setType((reactphysics3d::BodyType)body_type_);
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

            while (rigid_body_->getNbColliders())
            {
                rigid_body_->removeCollider(rigid_body_->getCollider(0));
            }
            world->physics_world_->destroyRigidBody(rigid_body_);
        }
    }
}

void MeshComponent::set_mesh(const Shared<StaticMesh>& mesh)
{
    if (mesh_ == mesh) return;

    mesh_ = mesh;
    
    if (auto owner = get_owner())
    {        
        if (auto world = owner->get_world())
        {
            if (ogre_entity_)
            {
                owner->scene_node_->detachObject(ogre_entity_);
                world->manager_->destroyEntity(ogre_entity_);
            }

            while (rigid_body_->getNbColliders())
            {
                rigid_body_->removeCollider(rigid_body_->getCollider(0));
            }

            ogre_entity_ = nullptr;

            setup_new_mesh(mesh);
        }
    }
}

uint MeshComponent::get_material_count() const
{
    if (ogre_entity_)
    {
        return ogre_entity_->getNumSubEntities();
    }

    return 0;
}

void MeshComponent::set_material(const Shared<Material>& material, uint material_slot)
{
    if (ogre_entity_)
    {
        if (material_slot < ogre_entity_->getNumSubEntities())
        {
            ogre_entity_->getSubEntity(material_slot)->setMaterial(material->ogre_material_ ? material->ogre_material_ : Ogre::MaterialManager::getSingleton().getByName("BaseWhite"));
        }
    }
}

Shared<Material> MeshComponent::get_material(uint material_slot) const
{
    if (ogre_entity_)
    {
        if (material_slot < ogre_entity_->getNumSubEntities())
        {
            Shared<Material> result = MakeShared<Material>();
            result->ogre_material_ = ogre_entity_->getSubEntity(material_slot)->getMaterial();
            return result;
        }
    }

    return nullptr;
}

void MeshComponent::set_material_parameter(Quaternion value, uint material_slot, uint parameter_index)
{
    if (ogre_entity_)
    {
        if (material_slot < ogre_entity_->getNumSubEntities())
        {
            ogre_entity_->getSubEntity(material_slot)->setCustomParameter(parameter_index, cast_object<Ogre::Vector4>(value));
        }
    }
}

void MeshComponent::set_body_type(PhysicalBodyType body_type)
{
    if (body_type_ == body_type) return;

    body_type_ = body_type;

    if (rigid_body_)
    {
        rigid_body_->setType((reactphysics3d::BodyType)body_type);
    }
}

void MeshComponent::set_visibility(bool state)
{
    if (is_visible_ == state) return;

    is_visible_ = state;

    if (ogre_entity_)
    {
        for (uint i = 0; i < ogre_entity_->getNumSubEntities(); i++)
        {
            ogre_entity_->getSubEntity(i)->setVisible(state);
        }
    }
}

void MeshComponent::setup_new_mesh(const Shared<StaticMesh>& mesh)
{
    if (mesh)
    {
        if (auto owner = get_owner())
        {
            if (auto world = owner->get_world())
            {
                ogre_entity_ = world->manager_->createEntity(mesh_->name.c());

                if (!is_visible_)
                {
                    for (uint i = 0; i < ogre_entity_->getNumSubEntities(); i++)
                    {
                        ogre_entity_->getSubEntity(i)->setVisible(is_visible_);
                    }
                }
                
                ogre_entity_->setCastShadows(true);
                owner->scene_node_->attachObject(ogre_entity_);

                for (auto& collision : mesh->collisions_)
                {
                    rigid_body_->addCollider(collision.collision->get_collider_shape(), reactphysics3d::Transform(cast_object<reactphysics3d::Vector3>(collision.location), cast_object<reactphysics3d::Quaternion>(collision.rotation)));
                }
            }
        }
    }
}
