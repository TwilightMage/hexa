#include "DefaultWorldGeneratorInfo.h"

#include "DefaultWorldGenerator.h"

String DefaultWorldGeneratorInfo::get_name() const
{
    return "default";
}

Shared<WorldGenerator> DefaultWorldGeneratorInfo::create_generator() const
{
    return MakeShared<DefaultWorldGenerator>();
}
