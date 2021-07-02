#include "RendererParameters.h"

RendererParameterApplier::RendererParameterApplier(uint count, uint item_size)
    : count(count)
    , item_size(item_size)
{
    data = malloc(count * item_size);
}

RendererParameterApplier::~RendererParameterApplier()
{
    free(data);
}

void RendererParameterApplier::put(RendererParameterBase* parameter, uint index)
{
    parameter->write_data(((byte*)data) + index * item_size);
}
