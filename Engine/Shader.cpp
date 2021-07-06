#include "Shader.h"

#include "File.h"
#include "Game.h"
#include "Logger.h"
#include "Regex.h"
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

Shared<Shader> Shader::compile(const String& name, const List<Path>& paths)
{
    Map<ShaderType, Path> path_type_map;
    
    for (auto& path : paths)
    {
        const auto absolute_path = path.get_absolute_string();
        if (!path.exists())
        {
            print_error("Shader", "Unable to find shader at path %s for shader program %s", absolute_path.c(), name.c());
            return nullptr;
        }

        ShaderType found_type = NONE;
        for (auto& type : shader_type_meta)
        {
            if (path.extension == type.value.format)
            {
                found_type = type.key;
                break;
            }
        }

        if (found_type == NONE)
        {
            print_error("Shader", "Unknown shader extension %s for shader %s", path.extension.c(), name.c());
            return nullptr;
        }

        if (path_type_map.contains(found_type))
        {
            print_error("Shader", "Multiple shaders of same type is not allowed! file %s for shader %s", absolute_path.c(), name.c());
            return nullptr;
        }

        path_type_map[found_type] = path;
    }
    
    const auto result = MakeShared<Shader>(name);

    Map<String, String> defines;

    for (auto& path_type : path_type_map)
    {
        result->shaders_[path_type.key] = compile_shader(path_type.value, path_type.key, defines);
    }

    if (defines.contains("transparency")) result->transparency_ = String::parse<bool>(defines["transparency"]);
    
    List<String> problem_shaders;
    for (auto& kvp : result->shaders_)
    {
        if (kvp.value == -1)
        {
            problem_shaders.add(shader_type_meta.at(kvp.key).name);
        }
    }
    
    if (problem_shaders.length() > 0)
    {
        for (auto& kvp : result->shaders_)
        {
            glDeleteShader(kvp.value);
        }
        print_error("Shader", "Unable to compile shader program %s because of problems with shaders: %s", name.c(), String::join(problem_shaders, ", ").c());
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
            char* param_name = new char[name_len_max];
            int name_len;
            int layout_size;
            uint type;
            glGetActiveUniform(result->program_, i, name_len_max, &name_len, &layout_size, &type, param_name);
            const uint layout = glGetUniformLocation(result->program_, param_name);
            String name_str = String(param_name, name_len);
            delete[] param_name;
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
                    result->instance_uniforms_.add(UniformParam(layout_size, *type_info, is_array, name_str, layout));
                }
                else
                {
                    result->global_uniforms_.add(UniformParam(layout_size, *type_info, is_array, name_str, layout));
                }
            }
            else
            {
                print_error("Shader", "Parameter %s has unsupported type", name_str.c());
                result->cleanup();
                return nullptr;
            }
        }

        GLint num_inputs = 0;
        glGetProgramiv(result->program_, GL_ACTIVE_ATTRIBUTES, &num_inputs);
        glGetProgramiv(result->program_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &name_len_max);
        for (int i = 0; i < num_inputs; i++)
        {
            char* param_name = new char[name_len_max];
            int name_len;
            int layout_size;
            uint type;
            glGetActiveAttrib(result->program_, i, name_len_max, &name_len, &layout_size, &type, param_name);
            const uint layout = glGetAttribLocation(result->program_, param_name);
            String name_str = String(param_name, name_len);
            delete[] param_name;
            if (name_str.starts_with("gl_")) continue;
            const auto shader_data_type = static_cast<GLTypeEnum>(type);
            if (const auto type_info = shader_type_info.find(shader_data_type))
            {
                result->vertex_params_.add(VertexParam(layout_size, *type_info, name_str, layout));
            }
            else
            {
                print_error("Shader", "Parameter %s has unsupported type", name_str.c());
                result->cleanup();
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
                print_error("Shader", "Shader %s does not contains info about vertices or empty vertex count", name.c());
                result->cleanup();
                return nullptr;
            }
        }
        
        Utils::check_gl_error();
        verbose("Shader", "Compiled shader %s", name.c());

        Game::instance_->shaders_[name] = result;
        
        return result;
    }
    
    return nullptr;
}

uint Shader::compile_shader(const Path& path, ShaderType shader_type, Map<String, String>& defines)
{
    const auto shader = glCreateShader(shader_type_meta.at(shader_type).gl_type);
    const auto shader_code = File::read_file(path).trim();
    char* shader_code_c = shader_code.c_copy();
    glShaderSource(shader, 1, &shader_code_c, nullptr);
    delete shader_code_c;
    glCompileShader(shader);

    GLint compile_log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &compile_log_length);

    if (compile_log_length > 0)
    {
        std::vector<GLchar> compile_log(compile_log_length);
        glGetShaderInfoLog(shader, compile_log_length, &compile_log_length, compile_log.data());

        auto log_lines = String(compile_log.data(), compile_log_length - 1).split('\n', true);

        for (auto& line : log_lines)
        {
            if (auto warning_capt = Regex("^(\\d+\\(\\d+\\)) : warning (.*)").capture(line))
            {
                line = CONSOLE_YELLOW + line + CONSOLE_RESET;
            }
            else if (auto error_capt = Regex("^(\\d+\\(\\d+\\)) : error (.*)").capture(line))
            {
                line = CONSOLE_RED + line + CONSOLE_RESET;
            }
            else
            {
                line = CONSOLE_WHITE + line + CONSOLE_RESET;
            }
        }

        verbose("Shader", "glsl compiler output for shader %s:\n%s", path.get_absolute_string().c(), String::join(log_lines, "\n").c());
    }
    
    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if(is_compiled == GL_FALSE)
    {
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
