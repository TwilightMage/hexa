#include "Shader.h"

#include "File.h"
#include "Game.h"

uint Shader::get_program() const
{
    return program_;
}

const struct Shader::Meta& Shader::get_meta() const
{
    return shader_meta_;
}

const String& Shader::get_name() const
{
    return name;
}

void Shader::map_params()
{
    glLinkProgram(program_);
    for (auto& vertex_param : shader_meta_.vertex_params)
    {
        vertex_param.id = glGetAttribLocation(program_, vertex_param.name.c());
        if (vertex_param.id != GL_INVALID_INDEX || true)
        {
            glEnableVertexAttribArray(vertex_param.id);
            glVertexAttribPointer(vertex_param.id, vertex_param.size, vertex_param.type, GL_FALSE, shader_meta_.vertex_param_size, reinterpret_cast<void*>(static_cast<uint64>(vertex_param.offset)));
        }
    }

    for (auto& uniform_param : shader_meta_.uniform_params)
    {
        if (!uniform_param.name.is_empty())
        {
            uniform_param.id = glGetUniformLocation(program_, uniform_param.name.c());
        }
        else
        {
            uniform_param.id = GL_INVALID_INDEX;
        }
    }
}

void Shader::cleanup()
{
    glLinkProgram(program_);

    for (auto& kvp : shaders_)
    {
        glDetachShader(program_, kvp.second);
        glDeleteShader(kvp.second);
    }

    glDeleteProgram(program_);
}

Shared<Shader> Shader::compile(const Path& path, const Meta& shader_meta, int type_flags)
{
    const auto found = Game::instance_->shaders_.find(path.get_absolute_string());
    if (found != Game::instance_->shaders_.end())
    {
        return found->second;
    }
    
    if (type_flags != 0)
    {
        const auto result = MakeShared<Shader>();
        result->program_ = glCreateProgram();
        
        if (type_flags & VERTEX)
        {
            result->shaders_[VERTEX] = compile_shader(path, VERTEX);
        }

        if (type_flags & FRAGMENT)
        {
            result->shaders_[FRAGMENT] = compile_shader(path, FRAGMENT);
        }

        if (type_flags & GEOMETRY)
        {
            result->shaders_[GEOMETRY] = compile_shader(path, GEOMETRY);
        }

        if (type_flags & COMPUTE)
        {
            result->shaders_[COMPUTE] = compile_shader(path, COMPUTE);
        }

        List<String> problem_shaders;
        for (auto& kvp : result->shaders_)
        {
            if (kvp.second == -1)
            {
                problem_shaders.Add(shader_type_meta.at(kvp.first).name);
            }
        }
        
        if (problem_shaders.length() > 0)
        {
            for (auto& kvp : result->shaders_)
            {
                glDeleteShader(kvp.second);
            }
            print_error("Shader", "Unable to compile shader program %s because of problems with shaders: %s", path.get_absolute_string().c(), StringJoin(problem_shaders, ", ").c());
        }
        else
        {
            result->shader_meta_ = shader_meta;

            result->program_ = glCreateProgram();
            glLinkProgram(result->program_);
            for (auto& kvp : result->shaders_)
            {
                glAttachShader(result->program_, kvp.second);
            }

            verbose("Shader", "Compiled shader %s", path.get_absolute_string().c());

            Game::instance_->shaders_[path.get_absolute_string()] = result;
            
            return result;
        }
    }
    
    return nullptr;
}

uint Shader::compile_shader(const Path& path, type shader_type)
{
    Path filepath = path.to_string() + shader_type_meta.at(shader_type).format;

    if (filepath.exists())
    {
        const auto shader = glCreateShader(shader_type_meta.at(shader_type).gl_type);
        auto shader_code = File::read_file(filepath).trim().c_copy();
        glShaderSource(shader, 1, &shader_code, nullptr);
        delete shader_code;
        glCompileShader(shader);

        GLint is_compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
        if(is_compiled == GL_FALSE)
        {
            GLint max_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

            std::vector<GLchar> error_log(max_length);
            glGetShaderInfoLog(shader, max_length, &max_length, error_log.data());

            print_error("Shader", "Failed to compile shader %s:\n%s", filepath.get_absolute().to_string().c(), error_log.data());

            glDeleteShader(shader);
            return -1;
        }
    
        return shader;  
    }
    else
    {
        print_error("Shader", "Failed to compile %s shader %s: file does not exist", shader_type_meta.at(shader_type).name.c(), filepath.get_absolute().to_string().c());
    }

    return -1;
}
