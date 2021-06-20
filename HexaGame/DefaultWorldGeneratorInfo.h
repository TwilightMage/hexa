﻿#pragma once

#include "WorldGeneratorInfo.h"

class EXPORT DefaultWorldGeneratorInfo : public WorldGeneratorInfo
{
public:
    String get_name() const override;
    Shared<WorldGenerator> create_generator() const override;
};
