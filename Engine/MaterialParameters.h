#pragma once

#include <glad/glad.h>

#include "BasicTypes.h"
#include "Cubemap.h"
#include "Game.h"
#include "Map.h"
#include "Matrix4x4.h"
#include "Name.h"
#include "Quaternion.h"
#include "Texture.h"
#include "Vector2.h"

class MaterialParameterBase;

enum class GLTypeEnum
{
    None = 0,
    Float = GL_FLOAT,
    Vec2 = GL_FLOAT_VEC2,
    Vec3 = GL_FLOAT_VEC3,
    Vec4 = GL_FLOAT_VEC4,
    Double = GL_DOUBLE,
    Dvec2 = GL_DOUBLE_VEC2,
    Dvec3 = GL_DOUBLE_VEC3,
    Dvec4 = GL_DOUBLE_VEC4,
    Int = GL_INT,
    Ivec2 = GL_INT_VEC2,
    Ivec3 = GL_INT_VEC3,
    Ivec4 = GL_INT_VEC4,
    Uint = GL_UNSIGNED_INT,
    Uvec2 = GL_UNSIGNED_INT_VEC2,
    Uvec3 = GL_UNSIGNED_INT_VEC3,
    Uvec4 = GL_UNSIGNED_INT_VEC4,
    Bool = GL_BOOL,
    Bvec2 = GL_BOOL_VEC2,
    Bvec3 = GL_BOOL_VEC3,
    Bvec4 = GL_BOOL_VEC4,
    Mat2 = GL_FLOAT_MAT2,
    Mat3 = GL_FLOAT_MAT3,
    Mat4 = GL_FLOAT_MAT4,
    Mat2x3 = GL_FLOAT_MAT2x3,
    Mat2x4 = GL_FLOAT_MAT2x4,
    Mat3x2 = GL_FLOAT_MAT3x2,
    Mat3x4 = GL_FLOAT_MAT3x4,
    Mat4x2 = GL_FLOAT_MAT4x2,
    Mat4x3 = GL_FLOAT_MAT4x3,
    Dmat2 = GL_DOUBLE_MAT2,
    Dmat3 = GL_DOUBLE_MAT3,
    Dmat4 = GL_DOUBLE_MAT4,
    Dmat2x3 = GL_DOUBLE_MAT2x3,
    Dmat2x4 = GL_DOUBLE_MAT2x4,
    Dmat3x2 = GL_DOUBLE_MAT3x2,
    Dmat3x4 = GL_DOUBLE_MAT3x4,
    Dmat4x2 = GL_DOUBLE_MAT4x2,
    Dmat4x3 = GL_DOUBLE_MAT4x3,
    Sampler1D = GL_SAMPLER_1D,
    Sampler2D = GL_SAMPLER_2D,
    Sampler3D = GL_SAMPLER_3D,
    SamplerCube = GL_SAMPLER_CUBE,
    Sampler1DShadow = GL_SAMPLER_1D_SHADOW,
    Sampler2DShadow = GL_SAMPLER_2D_SHADOW,
    Sampler1DArray = GL_SAMPLER_1D_ARRAY,
    Sampler2DArray = GL_SAMPLER_2D_ARRAY,
    Sampler1DArrayShadow = GL_SAMPLER_1D_ARRAY_SHADOW,
    Sampler2DArrayShadow = GL_SAMPLER_2D_ARRAY_SHADOW,
    Sampler2DMS = GL_SAMPLER_2D_MULTISAMPLE,
    Sampler2DMSArray = GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
    SamplerCubeShadow = GL_SAMPLER_CUBE_SHADOW,
    SamplerBuffer = GL_SAMPLER_BUFFER,
    Sampler2DRect = GL_SAMPLER_2D_RECT,
    Sampler2DRectShadow = GL_SAMPLER_2D_RECT_SHADOW,
    Isampler1D = GL_INT_SAMPLER_1D,
    Isampler2D = GL_INT_SAMPLER_2D,
    Isampler3D = GL_INT_SAMPLER_3D,
    IsamplerCube = GL_INT_SAMPLER_CUBE,
    Isampler1DArray = GL_INT_SAMPLER_1D_ARRAY,
    Isampler2DArray = GL_INT_SAMPLER_2D_ARRAY,
    Isampler2DMS = GL_INT_SAMPLER_2D_MULTISAMPLE,
    Isampler2DMSArray = GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
    IsamplerBuffer = GL_INT_SAMPLER_BUFFER,
    Isampler2DRect = GL_INT_SAMPLER_2D_RECT,
    Usampler1D = GL_UNSIGNED_INT_SAMPLER_1D,
    Usampler2D = GL_UNSIGNED_INT_SAMPLER_2D,
    Usampler3D = GL_UNSIGNED_INT_SAMPLER_3D,
    UsamplerCube = GL_UNSIGNED_INT_SAMPLER_CUBE,
    Usampler1DArray = GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
    Usampler2DArray = GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
    Usampler2DMS = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
    Usampler2DMSArray = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
    UsamplerBuffer = GL_UNSIGNED_INT_SAMPLER_BUFFER,
    Usampler2DRect = GL_UNSIGNED_INT_SAMPLER_2D_RECT,
    Image1D = GL_IMAGE_1D,
    Image2D = GL_IMAGE_2D,
    Image3D = GL_IMAGE_3D,
    Image2DRect = GL_IMAGE_2D_RECT,
    ImageCube = GL_IMAGE_CUBE,
    ImageBuffer = GL_IMAGE_BUFFER,
    Image1DArray = GL_IMAGE_1D_ARRAY,
    Image2DArray = GL_IMAGE_2D_ARRAY,
    Image2DMS = GL_IMAGE_2D_MULTISAMPLE,
    Image2DMSArray = GL_IMAGE_2D_MULTISAMPLE_ARRAY,
    Iimage1D = GL_INT_IMAGE_1D,
    Iimage2D = GL_INT_IMAGE_2D,
    Iimage3D = GL_INT_IMAGE_3D,
    Iimage2DRect = GL_INT_IMAGE_2D_RECT,
    IimageCube = GL_INT_IMAGE_CUBE,
    IimageBuffer = GL_INT_IMAGE_BUFFER,
    Iimage1DArray = GL_INT_IMAGE_1D_ARRAY,
    Iimage2DArray = GL_INT_IMAGE_2D_ARRAY,
    Iimage2DMS = GL_INT_IMAGE_2D_MULTISAMPLE,
    Iimage2DMSArray = GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
    Uimage1D = GL_UNSIGNED_INT_IMAGE_1D,
    Uimage2D = GL_UNSIGNED_INT_IMAGE_2D,
    Uimage3D = GL_UNSIGNED_INT_IMAGE_3D,
    Uimage2DRect = GL_UNSIGNED_INT_IMAGE_2D_RECT,
    UimageCube = GL_UNSIGNED_INT_IMAGE_CUBE,
    UimageBuffer = GL_UNSIGNED_INT_IMAGE_BUFFER,
    Uimage1DArray = GL_UNSIGNED_INT_IMAGE_1D_ARRAY,
    Uimage2DArray = GL_UNSIGNED_INT_IMAGE_2D_ARRAY,
    Uimage2DMS = GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,
    Uimage2DMSArray = GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
    AtomicUint = GL_UNSIGNED_INT_ATOMIC_COUNTER
};

struct GLType
{
    String name;
    uint c_size;
    uint gl_size;
    GLTypeEnum gl_type;
    GLTypeEnum gl_primitive_type;
    std::function<Shared<MaterialParameterBase>(const Name& name)> parameter_producer;
};

struct GLParamSignature
{
    const GLType* type;
    String name;
    bool is_instance;
};

class EXPORT MaterialParameterBase
{
public:
    virtual void write_data(void* dest) const = 0;
    virtual void reset() = 0;
    virtual void copy(MaterialParameterBase* to) const = 0;

    Name name;
    const GLType* type;
};

template<typename T>
class MaterialParameter : public MaterialParameterBase
{
public:
    FORCEINLINE void write_data(void* dest) const override { *((T*)dest) = value; }
    FORCEINLINE void reset() override { value = T(); }
    FORCEINLINE void copy(MaterialParameterBase* to) const override { ((MaterialParameter*)to)->value = value; }
    
    T value;
};

template<>
class MaterialParameter<Shared<Texture>> : public MaterialParameterBase
{
public:
    FORCEINLINE void write_data(void* dest) const override { *((uint64*)dest) = value ? value->get_handle_arb() : 0; }
    FORCEINLINE void reset() override { value = nullptr; }
    FORCEINLINE void copy(MaterialParameterBase* to) const override { (( MaterialParameter<Shared<Texture>>*)to)->value = value; }
    
    TextureSlot value;
};

template<>
class MaterialParameter<Shared<Cubemap>> : public MaterialParameterBase
{
public:
    FORCEINLINE void write_data(void* dest) const override { *((uint*)dest) = value ? value->get_gl_id() : 0; }
    FORCEINLINE void reset() override { value = nullptr; }
    FORCEINLINE void copy(MaterialParameterBase* to) const override { (( MaterialParameter<Shared<Cubemap>>*)to)->value = value; }
    
    Shared<Cubemap> value;
};

#define GLTYPE(T, primitive_type, size, c_size, c_type) { GLTypeEnum::T, new GLType { #T, c_size, size, GLTypeEnum::T, GLTypeEnum::primitive_type, [](const Name& name)->Shared<MaterialParameterBase>{ auto result = MakeShared<MaterialParameter<c_type>>(); result->name = Name(name); result->type = shader_type_info[GLTypeEnum::T]; return result; } } }
const Map<GLTypeEnum, const GLType*> shader_type_info = {
    GLTYPE(Int,         Int,   1, sizeof(int),        int            ),
    GLTYPE(Uint,        Uint,  1, sizeof(uint),       uint           ),
    GLTYPE(Bool,        Bool,  1, sizeof(bool),       bool           ),
    GLTYPE(Float,       Float, 1, sizeof(float),      float          ),
    GLTYPE(Vec2,        Float, 2, sizeof(Vector2),    Vector2        ),
    GLTYPE(Vec3,        Float, 3, sizeof(Vector3),    Vector3        ),
    GLTYPE(Vec4,        Float, 4, sizeof(Quaternion), Quaternion     ),
    GLTYPE(Mat4,        None,  0, sizeof(Matrix4x4),  Matrix4x4      ),
    GLTYPE(Sampler2D,   None,  0, sizeof(uint64),     Shared<Texture>),
    GLTYPE(SamplerCube, None,  0, sizeof(uint),       Shared<Cubemap>)
};
#undef GLTYPE