#pragma once

#include <map>
#include <mutex>
#include <optional>

#include "EventBus.h"
#include "GameInfo.h"
#include "List.h"
#include "LogStream.h"
#include "Path.h"
#include "Renderer.h"
#include "String.h"
#include "Vector2.h"
#include "Version.h"

class Collision;
class ConvexMeshCollision;
class SaveGame;
class Settings;
class SpriteFont;
class UIElement;
class Image;
class Mod;
class Camera;
class GLFWwindow;
class IControllable;
class World;
class Texture;

namespace reactphysics3d
{
    class PhysicsCommon;
}

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
    friend Texture;
    friend Entity;
    friend UIElement;
    friend Collision;
    
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
    static const Shared<Settings>& get_settings();
    static const Shared<SaveGame>& get_save_game();

    static void new_log_record(ELogLevel level, const String& category, const String& message);

    static void call_on_main_thread(std::function<void()> func);

    static bool is_app_path_set();
    static const Path& get_app_path();

    static void use_renderer(const Weak<Renderer>& renderer);
    static void use_ui_renderer(const Weak<Renderer>& ui_renderer);

    static Shared<Shader> get_basic_shader();
    static Shared<Shader> get_basic_ui_shader();
    static Shared<Texture> get_white_pixel();
    static Shared<SpriteFont> get_default_font();

    static uint get_screen_width();
    static uint get_screen_height();

    static const Vector2& get_mouse_pos();
    static const Vector2& get_mouse_delta();
    static void lock_mouse();
    static void unlock_mouse();
    static void hide_mouse();
    static void show_mouse();
    static void add_ui(const Weak<UIElement>& ui);
    static float get_ui_scale();

    static bool is_loading_stage();
    static bool is_render_stage();
    
protected:
    virtual void init_game_info(GameInfo& out_info) = 0;
    virtual Shared<Settings> generate_settings_object();
    virtual Shared<SaveGame> generate_save_game_object(const String& profile_name);
    virtual void loading_stage();
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
    static void window_size_callback(GLFWwindow* window, int width, int height);

    static Game* instance_;

    List<String> args_;

    GameInfo info_;
    Shared<Settings> settings_;
    Shared<SaveGame> save_game_;

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

    // Assets
    std::map<String, Shared<Shader>> shaders_;
    std::map<String, Shared<Mesh>> meshes_;
    std::map<String, Shared<Texture>> textures_;

    // Game
    Version game_version_ = {0, 1, 0};
    float ui_scale_ = 2;
    Unique<EventBus> event_bus_;
    Shared<Shader> basic_shader_;
    Shared<Shader> basic_ui_shader_;
    Shared<Texture> white_pixel_;
    Shared<SpriteFont> default_font_;
    List<Shared<Mod>> mods_;
    
    // Game Play
    Shared<Camera> current_camera_;
    Shared<IControllable> current_controllable_;
    Shared<World> world_;

    // Core
    Unique<IRenderer> renderer_;
    Unique<IRenderer> ui_renderer_;
    Shared<reactphysics3d::PhysicsCommon> physics_;
    Shared<UIElement> ui_root_;
    Weak<UIElement> ui_under_mouse_;
    Weak<UIElement> pressed_ui_;
    List<std::function<void()>> main_thread_calls_;
    std::mutex main_thread_calls_mutex_;

    bool is_loading_stage_;
    bool is_render_stage_;
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
