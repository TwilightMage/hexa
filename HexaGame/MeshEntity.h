#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/Entity.h"
#include "Engine/Mesh.h"

EXTERN class EXPORT MeshEntity : public Entity
{
public:
    MeshEntity(const Weak<Mesh>& mesh);

    virtual void start() override;

private:
    Shared<Mesh> mesh_;
};
