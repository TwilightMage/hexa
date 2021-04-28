#include "Shader.h"

#include <glad/glad.h>

#include "File.h"
#include "Game.h"

uint Shader::get_program() const
{
    return program;
}

const struct Shader::meta& Shader::get_meta() const
{
    return shader_meta;
}

Shader* Shader::compile(const Path& frag, const Path& vert, const meta& shader_meta)
{    
    if (frag.Exists() && vert.Exists())
    {
        const auto result = new Shader();

        // Fragment shader
        const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        auto fragment_shader_code = File::ReadFile(frag).Trim().c_copy();
        glShaderSource(fragment_shader, 1, &fragment_shader_code, nullptr);
        delete fragment_shader_code;
        glCompileShader(fragment_shader);

        GLint is_compiled_fragment = 0;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled_fragment);
        if(is_compiled_fragment == GL_FALSE)
        {
            GLint max_length = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

            std::vector<GLchar> error_log(max_length);
            glGetShaderInfoLog(fragment_shader, max_length, &max_length, error_log.data());

            print_error("OpenGL", "Failed to compile fragment shader %s:\n%s", frag.get_absolute().ToString(), error_log.data());

            glDeleteShader(fragment_shader);
            return nullptr;
        }

        // Vertex shader
        const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        auto vert_shader_code = File::ReadFile(vert).Trim().c_copy();
        glShaderSource(vertex_shader, 1, &vert_shader_code, nullptr);
        delete vert_shader_code;
        glCompileShader(vertex_shader);

        GLint is_compiled_vertex = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled_vertex);
        if(is_compiled_vertex == GL_FALSE)
        {
            GLint max_length = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

            std::vector<GLchar> error_log(max_length);
            glGetShaderInfoLog(vertex_shader, max_length, &max_length, error_log.data());

            print_error("OpenGL", "Failed to compile vertex shader %s:\n%s", vert.get_absolute().ToString(), error_log.data());

            glDeleteShader(vertex_shader);
            return nullptr;
        }

        // Shader program
        result->program = glCreateProgram();
        glAttachShader(result->program, vertex_shader);
        glAttachShader(result->program, fragment_shader);

        glLinkProgram(result->program);
        result->shader_meta = shader_meta;
        for (auto& vertex_param : result->shader_meta.vertex_params)
        {
            vertex_param.id = glGetAttribLocation(result->program, vertex_param.name.c());
            if (vertex_param.id != GL_INVALID_INDEX)
            {
                glEnableVertexAttribArray(vertex_param.id);
                glVertexAttribPointer(vertex_param.id, vertex_param.size, vertex_param.type, GL_FALSE, shader_meta.vertex_param_size, reinterpret_cast<void*>(static_cast<uint64>(vertex_param.offset)));
            }
        }

        if (!result->shader_meta.uniform_param_name.IsEmpty())
        {
            result->shader_meta.uniform_param_id = glGetUniformLocation(result->program, result->shader_meta.uniform_param_name.c());
        }
        else
        {
            result->shader_meta.uniform_param_id = 0;
        }

        Game::instance_->shaders.Add(result);
        
        return result;
    }
    
    return nullptr;
}

void Shader::link() const
{
    glLinkProgram(program);
}
