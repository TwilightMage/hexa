#pragma once

#include <glad/glad.h>

#include "Map.h"
#include "Matrix4x4.h"
#include "Object.h"
#include "Path.h"
#include "MaterialParameters.h"

class EXPORT Shader : public Object
{
public: 
    struct UniformParam
    {
        uint layout_size;
        const GLType* type;
        bool is_array;
        Name name;
        uint layout;
    };

    struct VertexParam
    {
        uint layout_size;
        const GLType* type;
        Name name;
        uint layout;
        void* offset;
    };

    enum ShaderType
    {
        ST_NONE      = 0,
        ST_VERTEX    = 1,
        ST_FRAGMENT  = 2,
        ST_GEOMETRY  = 4,
        ST_COMPUTE   = 8
    };

    enum class ProgramType
    {
        None   = 0,
        Draw   = 1,
        Shadow = 2
    };

    struct ProgramData
    {
        void cleanup();
        
        uint program_id = 0;
        Map<ShaderType, uint> shaders;
        uint instance_count;
    };

    explicit Shader(const String& name);

    FORCEINLINE const String& get_name() const;

    Shared<const ProgramData> get_program_data(ProgramType type) const;
    
    void cleanup();

    static Shared<Shader> compile(const String& name, const List<Path>& paths, ProgramType types_to_compile);

    FORCEINLINE const SimpleMap<Name, UniformParam>& instance_uniforms() const { return instance_uniforms_; }
    FORCEINLINE const SimpleMap<Name, UniformParam>& global_uniforms() const { return global_uniforms_; }
    FORCEINLINE const List<VertexParam>& vertex_params() const { return vertex_params_; }

    FORCEINLINE bool transparency() const { return transparency_; }
    FORCEINLINE uint empty_vertex() const { return empty_vertex_; }
    FORCEINLINE bool no_face_cull() const { return no_face_cull_; }
    

private:
    struct type_meta
    {
        String name;
        String format;
        uint gl_type;
    };
    
    inline static const Map<ShaderType, type_meta> shader_type_meta = {
        {ST_VERTEX, {"vertex", ".vert", GL_VERTEX_SHADER}},
        {ST_FRAGMENT, {"fragment", ".frag", GL_FRAGMENT_SHADER}},
        {ST_GEOMETRY, {"geometry", ".geom", GL_GEOMETRY_SHADER}},
        {ST_COMPUTE, {"compute", ".comp", GL_COMPUTE_SHADER}}
    };

    inline static const Map<ProgramType, String> program_type_defines = {
        {ProgramType::Draw, "DRAW"},
        {ProgramType::Shadow, "SHADOW"}
    };

    static uint compile_shader(ProgramType program_type, ShaderType shader_type, const Path& path, Map<String, String>& defines);
    static Shared<ProgramData> compile_program(ProgramType program_type, const Map<ShaderType, Path>& path_type_map, const String& name, SimpleMap<Name, UniformParam>& instance_uniforms, SimpleMap<Name, UniformParam>& global_uniforms, List<VertexParam>& vertex_params, Map<String, String>& defines);

    Map<ProgramType, Shared<ProgramData>> programs_;

    SimpleMap<Name, UniformParam> instance_uniforms_;
    SimpleMap<Name, UniformParam> global_uniforms_;
    List<VertexParam> vertex_params_;

    bool transparency_ = false;
    uint empty_vertex_ = 0;
    bool no_face_cull_ = false;
};

IMPLEMENT_ENUM_FLAGS(Shader::ProgramType);