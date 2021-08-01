﻿#pragma once

#include <mutex>
#include <OGRE/Bites/OgreApplicationContext.h>
#include <OGRE/Bites/OgreTrays.h>
#include <OGRE/Bites/OgreWindowEventUtilities.h>

#include "EventBus.h"
#include "GameInfo.h"
#include "List.h"
#include "Map.h"
#include "Module.h"
#include "Path.h"
#include "Slot.h"
#include "SoundHandle.h"
#include "String.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Version.h"

namespace OgreBites {
    class TrayManager;
}

class CameraComponent;
class AudioChannel;
class Audio;
class Entity;
class StaticMesh;
class TextBlock;
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
class GLFWcursor;
class IControllable;
class World;
class Texture;

namespace reactphysics3d
{
    class PhysicsCommon;
}

namespace SoLoud
{
    class Soloud;
}

namespace Ogre
{
    class RenderWindow;
    class Root;

    namespace RTShader
    {
        class ShaderGenerator;
    }
}

class EXPORT Game : public OgreBites::ApplicationContext, public OgreBites::InputListener, OgreBites::WindowEventListener/*, public OgreBites::TrayListener*/, public Module
{
    friend AudioChannel;
    friend Audio;
    friend SoundHandle;
    friend World;
    friend StaticMesh;
    friend Texture;
    friend Entity;
    friend UIElement;
    friend Collision;
    friend SystemIO;
    friend Animation;
    
public:
    Game(const String& name, int argc, char* argv[]);
    ~Game();

    void launch();

    static void possess(const Shared<IControllable>& controllable);
    static void focus_ui(const Shared<UIInputElement>& ui_input_reciever);
    static void use_camera(const Shared<CameraComponent>& camera);
    
    static void open_world(const Shared<World>& world);
    static void close_world();
    static const Shared<World>& get_world();

    static const List<String>& get_args();

    static Game* get_instance();

    static const GameInfo& get_info();
    static const Shared<Settings>& get_settings();
    template<typename T>
    FORCEINLINE static Shared<T> get_settings() { return cast<T>(get_settings()); }
    static const Shared<SaveGame>& get_save_game();
    template<typename T>
    FORCEINLINE static Shared<T> get_save_game() { return cast<T>(get_save_game()); }
    static const Shared<EventBus>& get_event_bus();
    template<typename T>
    FORCEINLINE static Shared<T> get_event_bus() { return cast<T>(get_event_bus()); }

    static void call_on_main_thread(std::function<void()> func);

    static const Path& get_app_path();

    static const Shared<Texture>& get_white_pixel();
    static const Shared<SpriteFont>& get_default_font();
    static const Shared<AudioChannel>& get_general_channel();

    static uint get_screen_width();
    static uint get_screen_height();

    static const Vector2& get_mouse_pos();
    static const Vector2& get_mouse_delta();
    static void lock_mouse();
    static void unlock_mouse();
    static void hide_mouse();
    static void show_mouse();
    static void set_cursor_texture(const Shared<Texture>& tex, uint hotspot_x, uint hotspot_y);
    static void add_ui(const Shared<UIElement>& ui);
    static float get_ui_scale();
    static Vector3 get_un_projected_mouse();
    static float get_time();

    static bool is_loading_stage();
    static bool is_render_stage();

    void on_add_resource_directories(Set<String>& local, Set<String>& global) override;
    
protected:
    virtual void init_game_info(GameInfo& out_info) = 0;
    virtual Shared<Settings> generate_settings_object();
    virtual Shared<SaveGame> generate_save_game_object(const String& profile_name);
    virtual Shared<EventBus> generate_event_bus_object();
    virtual void loading_stage();
    virtual void start();
    virtual void tick(float delta_time);
    virtual void unloading_stage();

    bool windowClosing(Ogre::RenderWindow* rw) override;

    void setup() override;
    
private:
    void render_loop();
    
    void cleanup();

    void init_game();

    bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
    bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
    bool textInput(const OgreBites::TextInputEvent& evt) override;
    bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
    bool axisMoved(const OgreBites::AxisEvent& evt) override;
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
    void windowResized(Ogre::RenderWindow* rw) override;

    static Game* instance_;

    List<String> args_;

    GameInfo info_;
    Shared<Settings> settings_;
    Shared<SaveGame> save_game_;
    Shared<EventBus> event_bus_;

    // Common
    Path app_path_;
    bool has_mouse_pos_;
    Vector2 mouse_pos_;
    Vector2 last_mouse_pos_;
    Vector2 mouse_delta_;
    bool lock_mouse_;
    Vector3 un_projected_mouse_;
    float time_;
    bool close = false;
    
    // GLFW
    GLFWwindow* window_;
    GLFWcursor* cursor_;

    // Assets
    Map<String, Shared<StaticMesh>> meshes_;
    Map<String, Shared<Texture>> textures_;
    Map<String, Shared<Animation>> animations_;
    Map<String, Shared<Audio>> audios_;
    List<Shared<AudioChannel>> audio_channels_;
    
    // Game
    Version game_version_ = {0, 1, 0};
    float ui_scale_ = 2;
    Slot<Texture> white_pixel_;
    Shared<SpriteFont> default_font_;
    Shared<AudioChannel> general_channel_;
    List<Shared<Mod>> mods_;
    
    // Game Play
    Shared<CameraComponent> current_camera_;
    Shared<IControllable> current_controllable_;
    Shared<UIInputElement> ui_input_element_;
    Shared<World> world_;

    // Core
    Shared<reactphysics3d::PhysicsCommon> physics_;
    Shared<SoLoud::Soloud> soloud_;
    Ogre::Root* ogre_;
    Shared<OgreBites::TrayManager> ogre_ui_;
    Ogre::RTShader::ShaderGenerator* shader_generator_;
    Shared<UIElement> ui_root_;
    Weak<UIElement> ui_under_mouse_;
    Weak<UIElement> pressed_ui_;
    List<std::function<void()>> main_thread_calls_;
    std::mutex main_thread_calls_mutex_;

    bool is_loading_stage_;
    bool is_render_stage_;
};
