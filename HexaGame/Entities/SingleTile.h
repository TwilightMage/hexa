#pragma once

#include "Engine/Entity.h"

class SingleTile : public Entity
{
public:
    SingleTile();
    
    void on_start() override;

    FORCEINLINE const Shared<MeshComponent>& mesh() const { return mesh_; }

private:
    Shared<MeshComponent> mesh_;
};
