#include "Entity.h"

#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/engine/PhysicsCommon.h>


#include "Assert.h"
#include "Game.h"
#include "Mesh.h"
#include "GeometryEditor.h"
#include "Material3D.h"
#include "World.h"
#include "Physics/Collision.h"

Entity::Entity()
    : Object(typeid(this).name() + String(" entity"))
    , is_matrix_dirty_(true)
    , pending_kill_(false)
    , started_(false)
    , rigid_body_(nullptr)
    , collider_(nullptr)
{
    material_ = Game::get_basic_material_3d();
    material_changed();
}

Shared<World> Entity::get_world() const
{
    return world_.lock();
}

void Entity::on_start()
{
}

void Entity::on_tick(float delta_time)
{
}

void Entity::on_destroy()
{
}

void Entity::destroy()
{
    pending_kill_ = true;
}

bool Entity::is_started() const
{
    return started_;
}

void Entity::set_mesh(const Shared<Mesh>& new_mesh)
{
    mesh_ = new_mesh;
    material_instance_->set_mesh(new_mesh);
}

void Entity::clear_mesh()
{
    mesh_ = nullptr;
    material_instance_->set_mesh(nullptr);
}

void Entity::mark_matrix_dirty()
{
    is_matrix_dirty_ = true;
}

Vector3 Entity::get_position() const
{
    if (rigid_body_)
    {
        return cast_object<Vector3>(rigid_body_->getTransform().getPosition());
    }
    
    return position_;
}

void Entity::set_position(const Vector3& pos)
{
    if (rigid_body_)
    {
        auto trans = rigid_body_->getTransform();
        trans.setPosition(reactphysics3d::Vector3(pos.x, pos.y, pos.z));
        rigid_body_->setTransform(trans);
    }
    else
    {
        position_ = pos;
    }

    is_matrix_dirty_ = true;
}

void Entity::translate(const Vector3& translation)
{
    set_position(get_position() + translation);
}

Quaternion Entity::get_rotation() const
{
    if (rigid_body_)
    {
        return cast_object<Quaternion>(rigid_body_->getTransform().getOrientation());
    }
    
    return rotation_;
}

void Entity::set_rotation(const Quaternion& rot)
{
    if (rigid_body_)
    {
        auto trans = rigid_body_->getTransform();
        trans.setOrientation(reactphysics3d::Quaternion(rot.x, rot.y, rot.z, rot.w));
        rigid_body_->setTransform(trans);
    }
    else
    {
        rotation_ = rot;
    }

    is_matrix_dirty_ = true;
}

Vector3 Entity::get_scale() const
{
    return scale_;
}

void Entity::set_scale(const Vector3& scale)
{
    scale_ = scale;

    is_matrix_dirty_ = true;
}

void Entity::set_collision(const Shared<Collision>& collision, const Vector3& offset)
{
    remove_collision();

    const auto collider_shape = collision->get_collider_shape();

    Assert(collider_shape != nullptr);

    if (!rigid_body_)
    {
        print_warning("Physics", "Attempt to set collider for entity which is not a rigid body!");
        return;
    }
    
    collision_ = collision;
    collider_ = rigid_body_->addCollider(collider_shape, reactphysics3d::Transform(cast_object<reactphysics3d::Vector3>(offset), reactphysics3d::Quaternion::identity()));
    collider_->setCollisionCategoryBits(collision_mask_);
}

void Entity::remove_collision()
{
    if (!rigid_body_)
    {
        print_warning("Physics", "Attempt to remove collider from entity which is not a rigid body!");
        return;
    }
    
    if (collider_)
    {
        rigid_body_->removeCollider(collider_);
        collider_ = nullptr;
    }

    collision_ = nullptr;
}

void Entity::set_collision_mask(byte16 bits)
{
    collision_mask_ = bits;
    
    if (collider_)
    {
        collider_->setCollisionCategoryBits(bits);
    }
}

byte16 Entity::get_collision_mask() const
{
    return collision_mask_;
}

void Entity::set_gravity_enabled(bool state) const
{
    if (!rigid_body_)
    {
        print_warning("Physics", "Attempt to set gravity for entity which is not a rigid body!");
        return;
    }
    
    rigid_body_->enableGravity(state);
}

void Entity::make_body_static()
{
    if (!rigid_body_)
    {
        print_warning("Physics", "Attempt to make entity static which is not a rigid body!");
        return;
    }

    update_physically_dynamic();
    
    rigid_body_->setType(reactphysics3d::BodyType::STATIC);
}

void Entity::make_body_dynamic()
{
    if (!rigid_body_)
    {
        print_warning("Physics", "Attempt to make entity dynamic which is not a rigid body!");
        return;
    }

    update_physically_dynamic();
    
    rigid_body_->setType(reactphysics3d::BodyType::DYNAMIC);
}

void Entity::make_body_kinematic()
{
    if (!rigid_body_)
    {
        print_warning("Physics", "Attempt to make entity kinematic which is not a rigid body!");
        return;
    }

    update_physically_dynamic();
    
    rigid_body_->setType(reactphysics3d::BodyType::KINEMATIC);
}

void Entity::set_material(const Shared<Material3D>& material)
{
    if (!Check(material != nullptr, "Entity", "Cannot set material to nullptr")) return;

    material_ = material;

    material_changed();
}

Shared<Material3DInstance> Entity::get_material_instance() const
{
    return material_instance_;
}

void Entity::add_component(const Shared<EntityComponent>& component)
{
    Assert(component->get_owner() == nullptr);

    component->owner = weak_from_this();
    components_.add(component);
}

void Entity::remove_component(const Shared<EntityComponent>& component)
{
    if (components_.contains(component))
    {
        components_.remove(component);
        component->on_destroy();
        component->owner = null_weak(Entity);
    }
}

void Entity::remove_all_components()
{
    for (auto& component : components_)
    {
        component->on_destroy();
        component->owner = null_weak(Entity);
    }
    components_.clear();
}

void Entity::generate_components()
{
}

void Entity::modify_matrix_params(Vector3& position, Quaternion& rotation, Vector3& scale)
{
}

void Entity::cache_matrix()
{
    Vector3 position = get_position();
    Quaternion rotation = get_rotation();
    Vector3 scale = get_scale();
    modify_matrix_params(position, rotation, scale);
    if (model_parameter_) model_parameter_->value = Matrix4x4().translate(position).rotate(rotation).scale(scale);
    is_matrix_dirty_ = false;
}

void Entity::material_changed()
{
    auto new_material_instance = cast<Material3DInstance>(material_->create_instance());
    
    if (material_instance_)
    {
        new_material_instance->copy_parameters_from(material_instance_);
        material_instance_->destroy();
    }

    model_parameter_ = new_material_instance->get_parameter<Matrix4x4>("model");

    material_instance_ = new_material_instance;

    material_instance_->set_mesh(mesh_);
}

void Entity::start()
{
    started_ = true;

    update_physically_dynamic();
    
    on_start();
}

void Entity::tick(float delta_time)
{
    on_tick(delta_time);
}

void Entity::update_physically_dynamic()
{
    is_physically_dynamic_ = rigid_body_ == nullptr || rigid_body_->isSleeping() || rigid_body_->getType() == reactphysics3d::BodyType::KINEMATIC;
}
