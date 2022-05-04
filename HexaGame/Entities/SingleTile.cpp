#include "SingleTile.h"

#include <OGRE/OgreSceneNode.h>

#include "Engine/MeshComponent.h"

SingleTile::SingleTile()
    : Entity()
{
    mesh_ = create_component<MeshComponent>();
}

void SingleTile::on_start()
{
    make_body_static();
}