#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/Entity.h"
#include "Engine/Mesh.h"

EXTERN class EXPORT MeshEntity : public Entity
{
public:
    MeshEntity(const Weak<Mesh>& mesh);

    virtual void on_start() override;

private:
    Shared<Mesh> mesh_;
};
