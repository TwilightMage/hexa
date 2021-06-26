#pragma once

#include "Engine/Entity.h"

class ChunkMeshEntity : public Entity
{
public:
    void on_start() override;

protected:
    bool is_rigid_body() const override { return true; }
};
