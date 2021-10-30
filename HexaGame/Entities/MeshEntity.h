#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/Entity.h"
#include "Engine/StaticMesh.h"

class EXPORT MeshEntity : public Entity
{
public:
    MeshEntity(const Shared<StaticMesh>& mesh);

    void on_start() override;

    FORCEINLINE const Shared<MeshComponent>& mesh() const { return mesh_component_; }
    
private:
    Shared<StaticMesh> mesh_;
    Shared<MeshComponent> mesh_component_;
};
