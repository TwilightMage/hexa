#include "RendererParameters.h"

RendererParameterApplier::RendererParameterApplier(uint count, const GLType* data_type, uint location)
    : count(count)
    , data_type(data_type)
    , location(location)
{
    data = malloc(count * data_type->c_size);
}

RendererParameterApplier::~RendererParameterApplier()
{
    free(data);
}

void RendererParameterApplier::put(IRendererParameter* parameter, uint index)
{
    parameter->write_data(((byte*)data) + index * data_type->c_size);
}

void RendererParameterApplier::apply() const
{
    switch (data_type->gl_type)
    {
    case GLTypeEnum::Int:
        glUniform1iv(location, count, (int*)data);
        break;
                
    case GLTypeEnum::Uint:
        glUniform1uiv(location, count, (uint*)data);
        break;
                
    case GLTypeEnum::Bool:
        #ifdef glUniform1bv
        glUniform1bv(location, count, (bool*)data);
        #endif
        break;

    case GLTypeEnum::Float:
        glUniform1fv(location, count, (float*)data);
        break;

    case GLTypeEnum::Vec2:
        glUniform2fv(location, count, (float*)data);
        break;

    case GLTypeEnum::Vec3:
        glUniform3fv(location, count, (float*)data);
        break;

    case GLTypeEnum::Vec4:
        glUniform4fv(location, count, (float*)data);
        break;

    case GLTypeEnum::Mat4:
        glUniformMatrix4fv(location, count, GL_FALSE, (float*)data);
        break;

    case GLTypeEnum::Sampler2D:
        glUniformHandleui64vARB(location, count, (uint64*)data);
        break;
    default:
        break;
    }
}
