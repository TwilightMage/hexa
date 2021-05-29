#pragma once

#include <map>

#include "Path.h"
#include "Object.h"

#include <glad/glad.h>

EXTERN class EXPORT Shader : public Object
{
public:
    struct Meta
    {
        struct VertexParam
        {
            String name;
            uint offset;
            uint size;
            uint type;
            uint id;
        };

        struct UniformParam
        {
            String name;
            uint id;
        };

        uint vertex_param_size;
        List<VertexParam> vertex_params;

        List<UniformParam> uniform_params;

        uint instance_count = 1;

        bool transparency = false;
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

    void map_params();

    void cleanup();
    
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
    std::map<type, uint> shaders_;
};
