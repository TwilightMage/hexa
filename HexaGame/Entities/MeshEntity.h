#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/Entity.h"
#include "Engine/Mesh.h"

class EXPORT MeshEntity : public Entity
{
public:
    MeshEntity(const Weak<Mesh>& mesh);

    void on_start() override;

private:
    Shared<Mesh> mesh_;
};
