#include "Shader.h"

#include "File.h"
#include "Game.h"
#include "Utils.h"

Shader::Shader(const String& name)
    : Object(name)
{
}

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

void Shader::cleanup()
{
    glLinkProgram(program_);

    for (auto& kvp : shaders_)
    {
        glDetachShader(program_, kvp.value);
        glDeleteShader(kvp.value);
    }

    glDeleteProgram(program_);
}

bool Shader::check_uniform_presence(const String& name, ShaderDataType type, bool instance) const
{
    if (auto found = instance ? instance_uniforms_.find(name) : global_uniforms_)
    {
        return found->type == type;
    }

    return false;
}

Shared<Shader> Shader::compile(const Path& path, const Meta& shader_meta, int type_flags)
{
    if (const auto found = Game::instance_->shaders_.find(path.get_absolute_string()))
    {
        return *found;
    }
    
    if (type_flags != 0)
    {
        const auto result = MakeShared<Shader>(path.filename);
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
            if (kvp.value == -1)
            {
                problem_shaders.Add(shader_type_meta.at(kvp.x).name);
            }
        }
        
        if (problem_shaders.length() > 0)
        {
            for (auto& kvp : result->shaders_)
            {
                glDeleteShader(kvp.value);
            }
            print_error("Shader", "Unable to compile shader program %s because of problems with shaders: %s", path.get_absolute_string().c(), String::join(problem_shaders, ", ").c());
        }
        else
        {
            result->shader_meta_ = shader_meta;

            result->program_ = glCreateProgram();
            glLinkProgram(result->program_);
            for (auto& kvp : result->shaders_)
            {
                glAttachShader(result->program_, kvp.value);
            }

            glLinkProgram(result->program_);

            int name_len_max = 32;
            
            int num_uniforms = 0;
            glGetProgramiv(result->program_, GL_ACTIVE_UNIFORMS, &num_uniforms);
            glGetProgramiv(result->program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_len_max);
            for (int i = 0; i < num_uniforms; i++)
            {
                char* name = new char[name_len_max];
                int name_len;
                int size;
                uint type;
                glGetActiveUniform(result->program_, i, name_len_max, &name_len, &size, &type, name);
                String name_str = String(name, name_len);
                bool is_array = false;
                if (name_str.ends_with("[0]"))
                {
                    name_str = name_str.substring(0, name_str.length() - 3);
                    is_array = true;
                }
                const uint layout = glGetUniformLocation(result->program_, name);
                if (name_str.starts_with("INST_"))
                {
                    name_str = name_str.substring(5);
                    result->instance_uniforms_.insert(name_str, UniformParam(size, static_cast<ShaderDataType>(type), is_array, name_str, layout));
                }
                else
                {
                    result->global_uniforms_.insert(name_str, UniformParam(size, static_cast<ShaderDataType>(type), is_array, name_str, layout));
                }
                delete[] name;
            }

            GLint num_inputs = 0;
            glGetProgramiv(result->program_, GL_ACTIVE_ATTRIBUTES, &num_inputs);
            glGetProgramiv(result->program_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &name_len_max);
            for (int i = 0; i < num_inputs; i++)
            {
                char* name = new char[name_len_max];
                int name_len;
                int size;
                uint type;
                const uint layout = glGetAttribLocation(result->program_, name);
                glGetActiveAttrib(result->program_, i, name_len_max, &name_len, &size, &type, name);
                result->vertex_params_.Add(VertexParam(size,static_cast<ShaderDataType>(type), String(name, name_len), layout));
                delete[] name;
            }
            result->vertex_params_.sort([](const VertexParam& a, const VertexParam& b) -> bool
            {
                return a.layout < b.layout;
            });
            uint vertex_param_offset = 0;
            for (auto& vertex_param : result->vertex_params_)
            {
                vertex_param.offset = (void*)(uint64)vertex_param_offset;
                vertex_param_offset += shader_type_info[vertex_param.type].c_size;
            }

            Utils::check_gl_error();
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
