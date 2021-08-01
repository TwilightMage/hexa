#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/Entity.h"
#include "Engine/StaticMesh.h"

class EXPORT MeshEntity : public Entity
{
public:
    MeshEntity(const Shared<StaticMesh>& mesh);

    void on_start() override;

private:
    Shared<StaticMesh> mesh_;
};
