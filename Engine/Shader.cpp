#include "Shader.h"

#include "File.h"
#include "Game.h"
#include "Logger.h"
#include "Regex.h"
#include "Set.h"
#include "Utils.h"

void Shader::ProgramData::cleanup()
{
    glLinkProgram(program_id);

    for (auto& kvp : shaders)
    {
        glDetachShader(program_id, kvp.value);
        glDeleteShader(kvp.value);
    }

    glDeleteProgram(program_id);
}

Shader::Shader(const String& name)
    : Object(name)
{
}

const String& Shader::get_name() const
{
    return name;
}

Shared<const Shader::ProgramData> Shader::get_program_data(ProgramType type) const
{
    if (const auto program = programs_.find(type))
    {
        return *program;
    }
    
    return nullptr;
}

void Shader::cleanup()
{
    for (auto& program : programs_)
    {
        program.value->cleanup();
    }
    programs_.clear();
}

Shared<Shader> Shader::compile(const String& name, const List<Path>& paths, ProgramType types_to_compile)
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

        ShaderType found_type = ST_NONE;
        for (auto& type : shader_type_meta)
        {
            if (path.extension == type.value.format)
            {
                found_type = type.key;
                break;
            }
        }

        if (found_type == ST_NONE)
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

    Set<ProgramType> types_to_compile_set;
    
    if (!!(types_to_compile | ProgramType::Draw))
    {
        types_to_compile_set.add(ProgramType::Draw);
    }
    if (!!(types_to_compile | ProgramType::Shadow))
    {
        types_to_compile_set.add(ProgramType::Shadow);
    }

    Map<String, String> defines;
    
    for (auto type_to_compile : types_to_compile_set)
    {
        SimpleMap<Name, UniformParam> instance_uniforms;
        SimpleMap<Name, UniformParam> global_uniforms;
        List<VertexParam> vertex_params;
        
        if (const auto program = compile_program(type_to_compile, path_type_map, name, instance_uniforms, global_uniforms, vertex_params, defines))
        {
            if (result->instance_uniforms_.size() == 0)
            {
                result->instance_uniforms_ = instance_uniforms;
            }

            if (result->global_uniforms_.size() == 0)
            {
                result->global_uniforms_ = global_uniforms;
            }

            if (result->vertex_params_.length() == 0)
            {
                result->vertex_params_ = vertex_params;
            }
            
            if (instance_uniforms == result->instance_uniforms_ && global_uniforms == result->global_uniforms_ && vertex_params == result->vertex_params_)
            {
                result->programs_[type_to_compile] = program;
            }
            else
            {
                print_error("Shader", "Unable to compile shader program %s. All program variants must have same parameter set");
                return nullptr;
            }
        }
        else
        {
            result->cleanup();
            return nullptr;
        }
    }

    if (defines.contains("transparency")) result->transparency_ = String::parse<bool>(defines["transparency"]);
    if (defines.contains("no_face_cull")) result->no_face_cull_ = String::parse<bool>(defines["no_face_cull"]);

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

    return result;
}

uint Shader::compile_shader(ProgramType program_type, ShaderType shader_type, const Path& path, Map<String, String>& defines)
{
    const auto shader = glCreateShader(shader_type_meta.at(shader_type).gl_type);
    const auto shader_code = String::format("#%s\n", program_type_defines[program_type].c()) + File::read_file(path).trim();
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

Shared<Shader::ProgramData> Shader::compile_program(ProgramType program_type, const Map<ShaderType, Path>& path_type_map, const String& name, SimpleMap<Name, UniformParam>& instance_uniforms, SimpleMap<Name, UniformParam>& global_uniforms, List<VertexParam>& vertex_params, Map<String, String>& defines)
{
    Shared<ProgramData> result = MakeShared<ProgramData>();

    for (auto& path_type : path_type_map)
    {
        result->shaders[path_type.key] = compile_shader(program_type, path_type.key, path_type.value, defines);
    }
    
    List<String> problem_shaders;
    for (auto& kvp : result->shaders)
    {
        if (kvp.value == -1)
        {
            problem_shaders.add(shader_type_meta.at(kvp.key).name);
        }
    }
    
    if (problem_shaders.length() > 0)
    {
        for (auto& kvp : result->shaders)
        {
            glDeleteShader(kvp.value);
        }
        print_error("Shader", "Unable to compile shader program %s because of problems with shaders: %s", name.c(), String::join(problem_shaders, ", ").c());
    }
    else
    {
        result->program_id = glCreateProgram();
        glLinkProgram(result->program_id);
        for (auto& kvp : result->shaders)
        {
            glAttachShader(result->program_id, kvp.value);
        }

        glLinkProgram(result->program_id);

        int name_len_max = 0;
        
        int num_uniforms = 0;
        glGetProgramiv(result->program_id, GL_ACTIVE_UNIFORMS, &num_uniforms);
        glGetProgramiv(result->program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_len_max);
        result->instance_count = 0;
        for (int i = 0; i < num_uniforms; i++)
        {
            char* param_name = new char[name_len_max];
            int name_len;
            int layout_size;
            uint type;
            glGetActiveUniform(result->program_id, i, name_len_max, &name_len, &layout_size, &type, param_name);
            const uint layout = glGetUniformLocation(result->program_id, param_name);
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
                    result->instance_count = result->instance_count == 0 ? layout_size : Math::min(result->instance_count, (uint)layout_size);
                    instance_uniforms[Name(name_str)] = UniformParam(layout_size, *type_info, is_array, Name(name_str), layout);
                }
                else
                {
                    global_uniforms[Name(name_str)] = UniformParam(layout_size, *type_info, is_array, Name(name_str), layout);
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
        glGetProgramiv(result->program_id, GL_ACTIVE_ATTRIBUTES, &num_inputs);
        glGetProgramiv(result->program_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &name_len_max);
        for (int i = 0; i < num_inputs; i++)
        {
            char* param_name = new char[name_len_max];
            int name_len;
            int layout_size;
            uint type;
            glGetActiveAttrib(result->program_id, i, name_len_max, &name_len, &layout_size, &type, param_name);
            const uint layout = glGetAttribLocation(result->program_id, param_name);
            String name_str = String(param_name, name_len);
            delete[] param_name;
            if (name_str.starts_with("gl_")) continue;
            const auto shader_data_type = static_cast<GLTypeEnum>(type);
            if (const auto type_info = shader_type_info.find(shader_data_type))
            {
                vertex_params.add(VertexParam(layout_size, *type_info, Name(name_str), layout));
            }
            else
            {
                print_error("Shader", "Parameter %s has unsupported type", name_str.c());
                result->cleanup();
                return nullptr;
            }
        }
        vertex_params.sort([](const VertexParam& a, const VertexParam& b) -> bool
        {
            return a.layout > b.layout;
        });
        uint vertex_param_offset = 0;
        for (auto& vertex_param : vertex_params)
        {
            vertex_param.offset = (void*)(uint64)vertex_param_offset;
            vertex_param_offset += vertex_param.type->c_size;
        }
        
        Utils::check_gl_error();
        verbose("Shader", "Compiled shader %s", name.c());

        Game::instance_->shaders_[name] = result;
        
        return result;
    }
    
    return nullptr;
}
