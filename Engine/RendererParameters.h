#pragma once

#include <glad/glad.h>

#include "BasicTypes.h"
#include "Quaternion.h"
#include "Texture.h"
#include "Vector2.h"

class RendererParameterBase;

class RendererParameterApplier
{
public:
    RendererParameterApplier(uint count, uint item_size);
    ~RendererParameterApplier();

    void put(RendererParameterBase* parameter, uint index);
    template<typename T>
    void put(const T& value, uint index)
    {
        *(T*)(((byte*)data) + index * item_size) = value;
    }

    template<typename T>
    void apply(uint location)
    {
        
    }
    
    void* data;
    uint count;
    uint item_size;
};

template<>
inline void RendererParameterApplier::apply<float>(uint location)
{
    glUniform1fv(location, count, (float*)data);
}

template<>
inline void RendererParameterApplier::apply<Vector2>(uint location)
{
    glUniform2fv(location, count, (float*)data);
}

template<>
inline void RendererParameterApplier::apply<Vector3>(uint location)
{
    glUniform3fv(location, count, (float*)data);
}

template<>
inline void RendererParameterApplier::apply<Quaternion>(uint location)
{
    glUniform4fv(location, count, (float*)data);
}

class RendererParameterBase
{
public:
    virtual uint size() const = 0;
    virtual void write_data(void* dest) const = 0;
    
    uint id;
};

template<typename T>
class RendererParameter : public RendererParameterBase
{
public:
    uint size() const override { return sizeof(T); }
    void write_data(void* dest) const override { *((T*)dest) = value; }
    
    T value;
};

template<>
class RendererParameter<Shared<Texture>> : public RendererParameterBase
{
public:
    uint size() const override { return sizeof(uint64); }
    void write_data(void* dest) const override { *((uint64*)dest) = value->get_handle_arb(); }
    
    TextureSlot value;
};

template<>
class RendererParameter<Color> : public RendererParameterBase
{
public:
    uint size() const override { return sizeof(Quaternion); }
    void write_data(void* dest) const override { *((Quaternion*)dest) = value.to_quaternion(); }
    
    Color value;
};