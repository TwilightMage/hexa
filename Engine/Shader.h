﻿#pragma once

#include <map>


#include "Path.h"

#include <glad/glad.h>

class Shader
{
public:
    struct meta
    {
        struct vertex_param
        {
            String name;
            uint offset;
            uint size;
            uint type;
            uint id;
        };

        struct uniform_param
        {
            String name;
            uint id;
        };

        uint vertex_param_size;
        List<vertex_param> vertex_params;

        List<uniform_param> uniform_params;
    };

    enum type
    {
        NONE      = 0,
        VERTEX    = 1,
        FRAGMENT  = 2,
        GEOMETRY  = 4,
        COMPUTE   = 8
    };

    uint get_program() const;
    const meta& get_meta() const;

    const String& get_name() const;

    void map_params();

    void cleanup();
    
    static Shared<Shader> compile(const Path& path, const meta& shader_meta, int type_flags);

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

    uint program_;
    meta shader_meta_;
    std::map<type, uint> shaders_;
    String name;
};
