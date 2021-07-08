#pragma once

#include <map>
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
        String name;
        uint layout;
    };

    struct VertexParam
    {
        uint layout_size;
        const GLType* type;
        String name;
        uint layout;
        void* offset;
    };

    enum ShaderType
    {
        NONE      = 0,
        VERTEX    = 1,
        FRAGMENT  = 2,
        GEOMETRY  = 4,
        COMPUTE   = 8
    };

    explicit Shader(const String& name);

    uint get_program() const;

    const String& get_name() const;

    FORCEINLINE bool get_transparency() const { return transparency_; }
    FORCEINLINE uint get_empty_vertex() const { return empty_vertex_; }
    FORCEINLINE bool get_no_face_cull() const { return no_face_cull_; }

    FORCEINLINE uint get_instance_count() const { return instance_count_; }
    
    FORCEINLINE const List<UniformParam>& get_instance_uniforms() const { return instance_uniforms_; }
    FORCEINLINE const List<UniformParam>& get_global_uniforms() const { return global_uniforms_; }
    FORCEINLINE const List<VertexParam>& get_vertex_params() const { return vertex_params_; }
    
    void cleanup();

    static Shared<Shader> compile(const String& name, const List<Path>& paths);

private:
    struct type_meta
    {
        String name;
        String format;
        uint gl_type;
    };
    inline static const Map<ShaderType, type_meta> shader_type_meta = {
        {VERTEX, {"vertex", ".vert", GL_VERTEX_SHADER}},
        {FRAGMENT, {"fragment", ".frag", GL_FRAGMENT_SHADER}},
        {GEOMETRY, {"geometry", ".geom", GL_GEOMETRY_SHADER}},
        {COMPUTE, {"compute", ".comp", GL_COMPUTE_SHADER}}
    };

    static uint compile_shader(const Path& path, ShaderType shader_type, Map<String, String>& defines);

    uint program_ = 0;
    Map<ShaderType, uint> shaders_;
    uint instance_count_;

    bool transparency_ = false;
    uint empty_vertex_ = 0;
    bool no_face_cull_ = false;

    List<UniformParam> instance_uniforms_;
    List<UniformParam> global_uniforms_;
    List<VertexParam> vertex_params_;
};
