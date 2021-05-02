#pragma once

#include <map>
#include <mutex>
#include <optional>



#include "EventBus.h"
#include "GameInfo.h"
#include "List.h"
#include "LogStream.h"
#include "Mesh.h"
#include "Path.h"
#include "Renderer.h"
#include "Shader.h"
#include "String.h"
#include "Vector2.h"


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
    ~Game();

    void launch();

    static void possess(const Weak<IControllable>& controllable);
    static void use_camera(const Weak<Camera>& camera);
    
    static void open_world(const Weak<World>& world);
    static void close_world();

    static const List<String>& get_args();

    static Game* get_instance();

    static const GameInfo& get_info();

    static void new_log_record(ELogLevel level, const String& category, const String& message);

    static bool is_app_path_set();
    static const Path& get_app_path();

    static Weak<Shader> get_basic_shader();

    static uint get_screen_width();
    static uint get_screen_height();

    static const Vector2& get_mouse_pos();
    static const Vector2& get_mouse_delta();
    static void lock_mouse();
    static void unlock_mouse();
    static void hide_mouse();
    static void show_mouse();

protected:
    virtual void init_game_info(GameInfo& out_info) = 0;
    virtual void start();
    virtual void tick(float delta_time);
    
private:
    void setup_window();
    void prepare();
    void render_loop();
    void cleanup();

    void init_game();

    void set_app_path(const Path& new_app_path);

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double x_pos, double y_pos);

    static Game* instance_;

    List<String> args_;

    GameInfo info_;

    // Logging
    LogStream log_stream_;
    std::mutex log_stream_mutex_;

    // Common
    Path app_path_;
    bool app_path_set_;
    bool has_mouse_pos_;
    Vector2 mouse_pos_;
    Vector2 last_mouse_pos_;
    Vector2 mouse_delta_;
    bool lock_mouse_;
    
    // GLFW
    GLFWwindow* window_;
    Shared<Shader> basic_shader_;
    Unique<EventBus> event_bus_;

    // Assets
    List<Shared<Shader>> shaders_;
    std::map<String, Shared<Mesh>> meshes_;

    // Game
    Shared<Camera> current_camera_;
    Shared<IControllable> current_controllable_;
    Shared<World> world_;

    // Rnder
    Unique<Renderer> renderer_;
};

template<typename... Args>
static void print_debug(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

	Game::new_log_record(ELogLevel::Debug, category, buffer);
}

template<typename... Args>
static void print_warning(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Game::new_log_record(ELogLevel::Warning, category, buffer);
}

template<typename... Args>
static void print_error(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Game::new_log_record(ELogLevel::Error, category, buffer);
}

template<typename... Args>
static void verbose(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Game::new_log_record(ELogLevel::Verbose, category, buffer);
}
