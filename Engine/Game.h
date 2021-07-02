#pragma once

#include <mutex>
#include <optional>

#include "EventBus.h"
#include "GameInfo.h"
#include "List.h"
#include "Map.h"
#include "Path.h"
#include "Slot.h"
#include "String.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Version.h"

class Renderer;
class Entity;
class Mesh;
class Shader;
class RendererUI;
class Renderer3D;
class TextBlock;
class ICamera;
class Animation;
class UIInputElement;
class SystemIO;
class Collision;
class ConvexMeshCollision;
class SaveGame;
class Settings;
class SpriteFont;
class UIElement;
class Image;
class Mod;
class GLFWwindow;
class IControllable;
class World;
class Texture;

namespace reactphysics3d
{
    class PhysicsCommon;
}

class EXPORT Game
{
    friend Renderer;
    friend World;
    friend Shader;
    friend Mesh;
    friend Texture;
    friend Entity;
    friend UIElement;
    friend Collision;
    friend SystemIO;
    friend Animation;
    
public:
    Game(int argc, char* argv[]);
    ~Game();

    void launch();

    static void possess(const Shared<IControllable>& controllable);
    static void focus_ui(const Shared<UIInputElement>& ui_input_reciever);
    static void use_camera(const Shared<ICamera>& camera);
    
    static void open_world(const Shared<World>& world);
    static void close_world();

    static const List<String>& get_args();

    static Game* get_instance();

    static const GameInfo& get_info();
    static const Shared<Settings>& get_settings();
    template<typename T>
    FORCEINLINE static T get_settings() { return cast<T>(get_settings()); }
    static const Shared<SaveGame>& get_save_game();
    template<typename T>
    FORCEINLINE static T get_save_game() { return cast<T>(get_save_game()); }
    static const Shared<EventBus>& get_event_bus();
    template<typename T>
    FORCEINLINE static T get_event_bus() { return cast<T>(get_event_bus()); }

    static void call_on_main_thread(std::function<void()> func);

    static bool is_app_path_set();
    static const Path& get_app_path();

    static Shared<Shader> get_basic_shader();
    static Shared<Shader> get_basic_ui_shader();
    static Shared<Renderer3D> get_basic_renderer_3d();
    static Shared<RendererUI> get_basic_renderer_ui();
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
    static void add_ui(const Shared<UIElement>& ui);
    static float get_ui_scale();
    static Vector3 get_un_projected_mouse();

    static bool is_loading_stage();
    static bool is_render_stage();
    
protected:
    virtual void init_game_info(GameInfo& out_info) = 0;
    virtual Shared<Settings> generate_settings_object();
    virtual Shared<SaveGame> generate_save_game_object(const String& profile_name);
    virtual Shared<EventBus> generate_event_bus_object();
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
    static void character_callback(GLFWwindow* window, uint codepoint);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void cursor_position_callback(GLFWwindow* window, double x_pos, double y_pos);
    static void window_size_callback(GLFWwindow* window, int width, int height);

    static Game* instance_;

    List<String> args_;

    GameInfo info_;
    Shared<Settings> settings_;
    Shared<SaveGame> save_game_;
    Shared<EventBus> event_bus_;

    // Common
    Path app_path_;
    bool app_path_set_;
    bool has_mouse_pos_;
    Vector2 mouse_pos_;
    Vector2 last_mouse_pos_;
    Vector2 mouse_delta_;
    bool lock_mouse_;
    Vector3 un_projected_mouse_;
    
    // GLFW
    GLFWwindow* window_;

    // Assets
    Map<String, Shared<Shader>> shaders_;
    Map<String, Shared<Mesh>> meshes_;
    Map<String, Shared<Texture>> textures_;
    Map<String, Shared<Animation>> animations_;
    List<Shared<Renderer>> renderers_;
    
    // Game
    Version game_version_ = {0, 1, 0};
    float ui_scale_ = 2;
    Shared<Shader> basic_shader_;
    Shared<Shader> basic_ui_shader_;
    Shared<Renderer3D> basic_renderer_3d_;
    Shared<RendererUI> basic_renderer_ui_;
    Slot<Texture> white_pixel_;
    Shared<SpriteFont> default_font_;
    List<Shared<Mod>> mods_;
    Shared<TextBlock> fps_display_;
    
    // Game Play
    Shared<ICamera> current_camera_;
    Shared<IControllable> current_controllable_;
    Shared<UIInputElement> ui_input_element_;
    Shared<World> world_;

    // Core
    Shared<reactphysics3d::PhysicsCommon> physics_;
    Shared<UIElement> ui_root_;
    Weak<UIElement> ui_under_mouse_;
    Weak<UIElement> pressed_ui_;
    List<std::function<void()>> main_thread_calls_;
    std::mutex main_thread_calls_mutex_;

    bool is_loading_stage_;
    bool is_render_stage_;
};
