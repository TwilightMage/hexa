#pragma once

#include <map>

#include "Path.h"
#include "Object.h"

#include <glad/glad.h>

#include "Game.h"
#include "Map.h"
#include "RendererParameters.h"

enum class ShaderDataType
{
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

struct ShaderDataTypeInfo
{
    uint c_size;
    std::function<Shared<RendererParameterBase>()> parameter_producer;
};

#define SHADER_DATA_TYPE_INFO(type, c_size, c_type) { ShaderDataType::type, { c_size, []()->Shared<RendererParameterBase>{ return MakeShared<RendererParameter<c_type>>(); } } }
const Map<ShaderDataType, ShaderDataTypeInfo> shader_type_info = {
    SHADER_DATA_TYPE_INFO(Float, sizeof(float), float),
    SHADER_DATA_TYPE_INFO(Vec2, sizeof(Vector2), Vector2),
    SHADER_DATA_TYPE_INFO(Vec3, sizeof(Vector3), Vector3),
    SHADER_DATA_TYPE_INFO(Vec4, sizeof(Quaternion), Quaternion),
    SHADER_DATA_TYPE_INFO(Mat4, sizeof(Matrix4x4), Matrix4x4),
    SHADER_DATA_TYPE_INFO(Usampler2D, sizeof(uint64), Shared<Texture>)
};
#undef SHADER_DATA_TYPE_INFO

class EXPORT Shader : public Object
{
    friend Renderer;
    
public:    
    struct Meta
    {
        uint instance_count = 1;

        bool transparency = false;
    };

    struct UniformParam
    {
        uint size;
        ShaderDataType type;
        bool is_array;
        String name;
        uint layout;
    };

    struct VertexParam
    {
        uint size;
        ShaderDataType type;
        String name;
        uint layout;
        void* offset;
    };

    enum type
    {
        NONE      = 0,
        VERTEX    = 1,
        FRAGMENT  = 2,
        GEOMETRY  = 4,
        COMPUTE   = 8
    };

    explicit Shader(const String& name);

    uint get_program() const;
    const Meta& get_meta() const;

    const String& get_name() const;

    void cleanup();

    bool check_uniform_presence(const String& name, ShaderDataType type, bool instance) const;
    
    static Shared<Shader> compile(const Path& path, const Meta& shader_meta, int type_flags);

private:
    struct type_meta
    {
        String name;
        String format;
        uint gl_type;
    };
    inline static const std::map<type, type_meta> shader_type_meta = {
        {VERTEX, {"vertex", ".vert", GL_VERTEX_SHADER}},
        {FRAGMENT, {"fragment", ".frag", GL_FRAGMENT_SHADER}},
        {GEOMETRY, {"geometry", ".geom", GL_GEOMETRY_SHADER}},
        {COMPUTE, {"compute", ".comp", GL_COMPUTE_SHADER}}
    };

    static uint compile_shader(const Path& path, type shader_type);

    uint program_ = 0;
    Meta shader_meta_ = {};
    Map<type, uint> shaders_;

    Map<String, UniformParam> instance_uniforms_;
    Map<String, UniformParam> global_uniforms_;
    List<VertexParam> vertex_params_;
};
