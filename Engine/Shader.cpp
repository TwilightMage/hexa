#include "Shader.h"

#include "File.h"
#include "Game.h"
#include "Logger.h"
#include "Utils.h"

Shader::Shader(const String& name)
    : Object(name)
{
}

uint Shader::get_program() const
{
    return program_;
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

bool Shader::check_uniform_presence(const String& name, GLTypeEnum type, bool instance) const
{
    if (const auto found = (instance ? instance_uniforms_ : global_uniforms_).find(name))
    {
        return found->type && found->type->gl_type == type;
    }

    return false;
}

Shared<Shader> Shader::compile(const Path& path, int type_flags)
{
    if (const auto found = Game::instance_->shaders_.find(path.get_absolute_string()))
    {
        return *found;
    }
    
    if (type_flags != 0)
    {
        const auto result = MakeShared<Shader>(path.filename);
        result->program_ = glCreateProgram();

        Map<String, String> defines;
        
        if (type_flags & VERTEX)
        {
            result->shaders_[VERTEX] = compile_shader(path, VERTEX, defines);
        }

        if (type_flags & FRAGMENT)
        {
            result->shaders_[FRAGMENT] = compile_shader(path, FRAGMENT, defines);
        }

        if (type_flags & GEOMETRY)
        {
            result->shaders_[GEOMETRY] = compile_shader(path, GEOMETRY, defines);
        }

        if (type_flags & COMPUTE)
        {
            result->shaders_[COMPUTE] = compile_shader(path, COMPUTE, defines);
        }

        if (defines.contains("transparency")) result->transparency_ = String::parse<bool>(defines["transparency"]);
        
        List<String> problem_shaders;
        for (auto& kvp : result->shaders_)
        {
            if (kvp.value == -1)
            {
                problem_shaders.Add(shader_type_meta.at(kvp.key).name);
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
            result->program_ = glCreateProgram();
            glLinkProgram(result->program_);
            for (auto& kvp : result->shaders_)
            {
                glAttachShader(result->program_, kvp.value);
            }

            glLinkProgram(result->program_);

            int name_len_max = 0;
            
            int num_uniforms = 0;
            glGetProgramiv(result->program_, GL_ACTIVE_UNIFORMS, &num_uniforms);
            glGetProgramiv(result->program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_len_max);
            result->instance_count_ = 0;
            for (int i = 0; i < num_uniforms; i++)
            {
                char* name = new char[name_len_max];
                int name_len;
                int layout_size;
                uint type;
                glGetActiveUniform(result->program_, i, name_len_max, &name_len, &layout_size, &type, name);
                const uint layout = glGetUniformLocation(result->program_, name);
                String name_str = String(name, name_len);
                delete[] name;
                bool is_array = layout_size > 1;
                if (name_str.ends_with("[0]"))
                {
                    name_str = name_str.substring(0, name_str.length() - 3);
                }
                const auto shader_data_type = static_cast<GLTypeEnum>(type);
                if (const auto type_info = shader_type_info.find(shader_data_type))
                {
                    if (name_str.starts_with("INST_"))
                    {
                        name_str = name_str.substring(5);
                        result->instance_count_ = result->instance_count_ == 0 ? layout_size : Math::min(result->instance_count_, (uint)layout_size);
                        result->instance_uniforms_.insert(name_str, UniformParam(layout_size, *type_info, is_array, name_str, layout));
                    }
                    else
                    {
                        result->global_uniforms_.insert(name_str, UniformParam(layout_size, *type_info, is_array, name_str, layout));
                    }
                }
                else
                {
                    print_error("Shader", "Parameter %s has unsupported type", name_str.c());
                    return nullptr;
                }
            }

            GLint num_inputs = 0;
            glGetProgramiv(result->program_, GL_ACTIVE_ATTRIBUTES, &num_inputs);
            glGetProgramiv(result->program_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &name_len_max);
            for (int i = 0; i < num_inputs; i++)
            {
                char* name = new char[name_len_max];
                int name_len;
                int layout_size;
                uint type;
                glGetActiveAttrib(result->program_, i, name_len_max, &name_len, &layout_size, &type, name);
                const uint layout = glGetAttribLocation(result->program_, name);
                String name_str = String(name, name_len);
                delete[] name;
                if (name_str.starts_with("gl_")) continue;
                const auto shader_data_type = static_cast<GLTypeEnum>(type);
                if (const auto type_info = shader_type_info.find(shader_data_type))
                {
                    result->vertex_params_.Add(VertexParam(layout_size, *type_info, name_str, layout));
                }
                else
                {
                    print_error("Shader", "Parameter %s has unsupported type", name_str.c());
                    return nullptr;
                }
            }
            result->vertex_params_.sort([](const VertexParam& a, const VertexParam& b) -> bool
            {
                return a.layout > b.layout;
            });
            uint vertex_param_offset = 0;
            for (auto& vertex_param : result->vertex_params_)
            {
                vertex_param.offset = (void*)(uint64)vertex_param_offset;
                vertex_param_offset += vertex_param.type->c_size;
            }

            if (result->vertex_params_.length() == 0)
            {
                if (defines.contains("empty_vertex"))
                {
                    result->empty_vertex_ = String::parse<uint>(defines["empty_vertex"]);
                }
                else
                {
                    print_error("Shader", "Shader %s does not contains info about vertices or empty vertex count", path.get_absolute_string().c());
                    return nullptr;
                }
            }
            
            Utils::check_gl_error();
            verbose("Shader", "Compiled shader %s", path.get_absolute_string().c());

            Game::instance_->shaders_[path.get_absolute_string()] = result;
            
            return result;
        }
    }
    
    return nullptr;
}

uint Shader::compile_shader(const Path& path, type shader_type, Map<String, String>& defines)
{
    Path filepath = path.to_string() + shader_type_meta.at(shader_type).format;

    if (filepath.exists())
    {
        const auto shader = glCreateShader(shader_type_meta.at(shader_type).gl_type);
        const auto shader_code = File::read_file(filepath).trim();
        char* shader_code_c = shader_code.c_copy();
        glShaderSource(shader, 1, &shader_code_c, nullptr);
        delete shader_code_c;
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

        for (auto line : shader_code.split('\n'))
        {
            line = line.trim();
            if (line.starts_with("#define"))
            {
                line = line.substring(7).trim();
                const int sep_pos = line.index_of(' ');
                if (sep_pos > 0)
                {
                    defines.insert(line.substring(0, sep_pos), line.substring(sep_pos + 1));
                }
                else
                {
                    defines.insert(line, "true");
                }
            }
        }
    
        return shader;  
    }
    else
    {
        print_error("Shader", "Failed to compile %s shader %s: file does not exist", shader_type_meta.at(shader_type).name.c(), filepath.get_absolute().to_string().c());
    }

    return -1;
}
