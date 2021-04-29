#pragma once

#include "Path.h"

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

        uint vertex_param_size;
        List<vertex_param> vertex_params;

        String uniform_param_name;
        uint uniform_param_id;
    };

    uint get_program() const;
    const meta& get_meta() const;

    void map_params();
    
    static Shared<Shader> compile(const Path& frag, const Path& vert, const meta& shader_meta);

private:
    uint program;
    meta shader_meta;
};
