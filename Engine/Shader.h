#pragma once

#include <map>
#include <glad/glad.h>

#include "Map.h"
#include "Matrix4x4.h"
#include "Object.h"
#include "Path.h"
#include "MaterialParameters.h"

class Material;

class EXPORT Shader : public Object
{
    friend Material;
    
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

    const String& get_name() const;

    FORCEINLINE bool get_transparency() const { return transparency_; }
    FORCEINLINE uint get_empty_vertex() const { return empty_vertex_; }

    void cleanup();

    bool check_uniform_presence(const String& name, GLTypeEnum type, bool instance) const;
    
    static Shared<Shader> compile(const Path& path, int type_flags);

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

    static uint compile_shader(const Path& path, type shader_type, Map<String, String>& defines);

    uint program_ = 0;
    Map<type, uint> shaders_;
    uint instance_count_;

    bool transparency_ = false;
    uint empty_vertex_ = 0;

    List<UniformParam> instance_uniforms_;
    List<UniformParam> global_uniforms_;
    List<VertexParam> vertex_params_;
};
