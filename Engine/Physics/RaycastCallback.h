#pragma once

#include <reactphysics3d/collision/RaycastInfo.h>

#include "RaycastResult.h"
#include "Engine/List.h"

class EXPORT RaycastCallback : public reactphysics3d::RaycastCallback
{
public:
    reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& raycastInfo) override;

    List<RaycastResult> results;
};
