#pragma once

#include <map>
#include <mutex>
#include <optional>


#include "GameInfo.h"
#include "List.h"
#include "LogStream.h"
#include "Mesh.h"
#include "Path.h"
#include "RenderDatabase.h"
#include "Shader.h"
#include "String.h"


class Camera;
class GLFWwindow;
class IControllable;
class World;

enum class ELogLevel
{
    Verbose,
    Debug,
    Warning,
    Error
};

EXTERN class EXPORT Game
{
    friend World;
    friend Shader;
    friend Mesh;
    
public:
    Game(int argc, char* argv[]);

    void launch();

    void possess(const Weak<IControllable>& controllable);
    void use_camera(const Weak<Camera>& camera);

    const List<String>& get_args() const;

    static Game* get_instance();

    const GameInfo& get_info() const;

    void new_log_record(ELogLevel level, const String& category, const String& message);

    static bool is_app_path_set();
    static const Path& get_app_path();

    Weak<Shader> get_basic_shader() const;

    static uint get_screen_width();
    static uint get_screen_height();

protected:
    virtual void init_game_info(GameInfo& out_info) = 0;

private:
    void setup_window();
    void prepare();
    void render_loop();

    void init_game();

    static void set_app_path(const Path& new_app_path);

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static Game* instance_;

    List<String> args_;

    GameInfo info_;

    LogStream log_stream_;
    std::mutex log_stream_mutex_;
    
    static Path app_path_;
    static bool app_path_set_;

    GLFWwindow* window_;
    Shared<Shader> basic_shader_;

    List<Shared<Shader>> shaders;
    List<Shared<Mesh>> meshes;
    
    render_database render_database;

    Shared<Camera> current_camera_;
    Shared<IControllable> current_controllable_;
    Shared<World> world_;
};

template<typename... Args>
static void print_debug(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

	Game::get_instance()->new_log_record(ELogLevel::Debug, category, buffer);
}

template<typename... Args>
static void print_warning(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Game::get_instance()->new_log_record(ELogLevel::Warning, category, buffer);
}

template<typename... Args>
static void print_error(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Game::get_instance()->new_log_record(ELogLevel::Error, category, buffer);
}

template<typename... Args>
static void verbose(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Game::get_instance()->new_log_record(ELogLevel::Verbose, category, buffer);
}