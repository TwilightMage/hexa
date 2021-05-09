#include "Game.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include "Camera.h"
#include "IControllable.h"
#include "Mod.h"
#include "Path.h"
#include "Quaternion.h"
#include "Renderer.h"
#include "Shader.h"
#include "SpriteFont.h"
#include "TextureAtlas.h"
#include "UIRenderer.h"
#include "World.h"
#include "ui/Image.h"
#include "ui/Panel.h"

Game* Game::instance_ = nullptr;

Game::Game(int argc, char* argv[])
    : log_stream_(DateTime::now(), argv[0])
	, event_bus_(new EventBus())
	, renderer_(new Renderer)
	, ui_renderer_(new UIRenderer)
	, physics_(new reactphysics3d::PhysicsCommon)
	, ui_root_(new UIElement)
{
	if (instance_)
	{
		throw std::runtime_error("Only one game instance may exist");
	}
	else
	{
		instance_ = this;
	}

	for (int i = 0; i < argc; i++)
	{
		args_.Add(argv[i]);
	}

	set_app_path(String(argv[0]));
}

void Game::launch()
{
	verbose("Game", "Launching...");

	if (!glfwInit())
	{
		print_error("Initialization", "Failed to initialize glfw");
	}
	
	init_game();
    setup_window();
    prepare();
    render_loop();
	cleanup();
}

void Game::possess(const Weak<IControllable>& controllable)
{
	if (instance_->current_controllable_)
	{
		instance_->current_controllable_->on_unpossess();
	}
	instance_->current_controllable_ = controllable.lock();
	instance_->current_controllable_->on_possess();
}

void Game::use_camera(const Weak<Camera>& camera)
{
	instance_->current_camera_ = camera.lock();
}

void Game::open_world(const Weak<World>& world)
{
	close_world();

	if (const auto world_ptr = world.lock())
	{
		instance_->world_ = world_ptr;
		instance_->world_->start();
		instance_->event_bus_->world_opened(world);
	}
}

void Game::close_world()
{
	if (instance_->world_)
	{
		instance_->world_->close();
		instance_->event_bus_->world_closed(instance_->world_);
		instance_->world_ = nullptr;
	}
}

const List<String>& Game::get_args()
{
	return instance_->args_;
}

Game* Game::get_instance()
{
	return instance_;
}

const GameInfo& Game::get_info()
{
	return instance_->info_;
}

void Game::new_log_record(ELogLevel level, const String& category, const String& message)
{
	static const char* levelNames[4] = { "Verbose", "Debug", "Warning", "Error" };
	static const char* levelColors[4] = { CONSOLE_WHITE, CONSOLE_CYAN, CONSOLE_YELLOW, CONSOLE_RED };

	instance_->log_stream_mutex_.lock();
	instance_->log_stream_ << levelColors[static_cast<int>(level)] << "[" << DateTime::now().to_string().c() << "] [" << levelNames[static_cast<int>(level)] << "] [" << category.c() << "] " << message.c() << CONSOLE_RESET << "\n";
	instance_->log_stream_mutex_.unlock();
}

bool Game::is_app_path_set()
{
	return instance_->app_path_set_;
}

const Path& Game::get_app_path()
{
	return instance_->app_path_;
}

void Game::use_renderer(const Weak<Renderer>& renderer)
{
	if (instance_->is_loading_stage_)
	{
		if (const auto& renderer_ptr = renderer.lock())
		{
			instance_->renderer_.reset(renderer_ptr.get());
		}
	}
}

void Game::use_ui_renderer(const Weak<Renderer>& ui_renderer)
{
	if (instance_->is_loading_stage_)
	{
		if (const auto& ui_renderer_ptr = ui_renderer.lock())
		{
			instance_->ui_renderer_.reset(ui_renderer_ptr.get());
		}
	}
}

Shared<Shader> Game::get_basic_shader()
{
	return instance_->basic_shader_;
}

Shared<Shader> Game::get_basic_ui_shader()
{
	return instance_->basic_ui_shader_;
}

Shared<Texture> Game::get_white_pixel()
{
	return instance_->white_pixel_;
}

Shared<SpriteFont> Game::get_default_font()
{
	return instance_->default_font_;
}

uint Game::get_screen_width()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

uint Game::get_screen_height()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

const Vector2& Game::get_mouse_pos()
{
	return instance_->mouse_pos_;
}

const Vector2& Game::get_mouse_delta()
{
	return instance_->mouse_delta_;
}

void Game::lock_mouse()
{
	instance_->lock_mouse_ = true;
}

void Game::unlock_mouse()
{
	instance_->lock_mouse_ = false;
}

void Game::hide_mouse()
{
	glfwSetInputMode(instance_->window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Game::show_mouse()
{
	glfwSetInputMode(instance_->window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Game::add_ui(const Weak<UIElement>& ui)
{
	instance_->ui_root_->add_child(ui);
}

bool Game::is_loading_stage()
{
	return instance_->is_loading_stage_;
}

bool Game::is_render_stage()
{
	return instance_->is_render_stage_;
}

void Game::start()
{
}

void Game::tick(float delta_time)
{
}

void Game::setup_window()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	ui_root_->set_size(Vector2(800, 600));
	ui_root_->mouse_detection_ = false;
	ui_root_->constructed_ = true;
	
	window_ = glfwCreateWindow(800, 600, get_info().title.c(), nullptr, nullptr);
	if (!window_)
	{
		print_error("Initialization", "Failed to create window");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}

void Game::prepare()
{
	glfwMakeContextCurrent(window_);
	gladLoadGL();
	glfwSwapInterval(1);
	
	glfwSetErrorCallback(error_callback);
	glfwSetWindowSizeLimits(window_, 800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetKeyCallback(window_, key_callback);
	glfwSetMouseButtonCallback(window_, mouse_button_callback);
	glfwSetCursorPosCallback(window_, cursor_position_callback);
	glfwSetWindowSizeCallback(window_, window_size_callback);
}

void Game::render_loop()
{
	const String renderer(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	verbose("OpenGL", "Active GPU: %s", renderer.c());
	glfwSetWindowTitle(window_, (get_info().title + " - " + renderer).c());
	
	const String opengl_version_string(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	verbose("OpenGL", "Detected version: %s", opengl_version_string.c());
	const Version opengl_version = opengl_version_string.substring(0, opengl_version_string.index_of(' '));
	if (opengl_version < Version(3, 0, 0))
	{
		print_error("OpenGL", "Unsupported version");
		return;
	}
	
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &TextureAtlas::max_size_);

	verbose("Mod Loader", "Searching for mods in %s/...", Path("mods").get_absolute_string().c());
	for (auto& path : Path("mods").list())
	{
		if (path.type == EPathType::Directory && Mod::verify_signature(path))
		{
			if (auto mod = Mod::load(path.get_child(path.filename + ".dll")))
			{
				mod->info_ = mod->load_mod_info(path.get_child(path.filename + ".meta"));
				if (mod->info_.target_game_version != game_version_)
				{
					print_error("Mod Loader", "Mod %s target game version is %s, but current game version is %s. This mod will be skipped", mod->info_.display_name.c(), mod->info_.target_game_version.to_string().c(), game_version_.to_string().c());
					continue;
				}
				mods_.Add(mod);
				verbose("Mod Loader", "Pre-loaded mod %s", mod->info_.display_name.c());
			}
			else
			{
				print_warning("Mod Loader", "Failed to pre-load mod %s", path.get_absolute_string().c());
			}
		}
	}

	// LOADING STAGE
	verbose("Game", "Loading stage...");
	is_loading_stage_ = true;

	// Load shaders
	Shader::Meta basic_shader_meta;
	basic_shader_meta.vertex_param_size = sizeof(Mesh::vertex);
	basic_shader_meta.vertex_params = {
		{"vPos", 0, 3, GL_FLOAT},
		{"vUV", sizeof(float) * 3, 2, GL_FLOAT},
		{"vCol", sizeof(float) * 5, 3, GL_FLOAT}
	};
	basic_shader_meta.instance_count = 230;
	basic_shader_ = Shader::compile("resources/engine/shaders/basic", basic_shader_meta, Shader::VERTEX | Shader::FRAGMENT);

	Shader::Meta basic_ui_shader_meta;
	basic_ui_shader_meta.instance_count = 146;
	basic_ui_shader_meta.transparency = true;
	basic_ui_shader_ = Shader::compile("resources/engine/shaders/basic_ui", basic_ui_shader_meta, Shader::VERTEX | Shader::FRAGMENT);

	white_pixel_ = MakeShared<Texture>("White Pixel", 1, 1, List<Color>::of(Color::white));

	default_font_ = SpriteFont::load_fnt("resources/hexagame/fonts/berkshire_swash.fnt");
	
	// Call load stage in mods
	for (auto& mod : mods_)
	{
		try
		{
			mod->loading_stage();
			verbose("Mod Loader", "Loaded mod %s", mod->info_.name.c());
		}
		catch (std::runtime_error err)
		{
			print_error("Mod Loader", "Failed to load mod %s: %s", mod->info_.name.c(), err.what());
			return;
		}
	}
	
	is_loading_stage_ = false;

	// Call load stage in mods
	verbose("Game", "Post-loading stage...");
	for (auto& mod : mods_)
	{
		try
		{
			mod->on_loaded(event_bus_.get());
			verbose("Mod Loader", "Post-loaded mod %s", mod->info_.name.c());
		}
		catch (std::runtime_error err)
		{
			print_error("Mod Loader", "Failed to post-load mod %s: %s", mod->info_.name.c(), err.what());
			return;
		}
	}

	verbose("Game", "Starting...");
	start();

	float last_delta_time = 0.0f;
	
	verbose("Game", "Entering game-loop...");
	while (!glfwWindowShouldClose(window_))
	{
		const auto tick_start = glfwGetTime();
		
		int width, height;
 
		glfwGetFramebufferSize(window_, &width, &height);
		
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mouse_delta_ = mouse_pos_ - last_mouse_pos_;
		if (lock_mouse_)
		{
			last_mouse_pos_ = mouse_pos_ = Vector2(static_cast<float>(width / 2), static_cast<float>(height / 2));
			glfwSetCursorPos(window_, width / 2, height / 2);
		}
		else
		{
			last_mouse_pos_ = mouse_pos_;
		}
		
		if (current_camera_ && current_camera_->owner)
		{
			if (last_delta_time > 0.0f)
			{
				tick(last_delta_time);
				world_->tick(last_delta_time);
			}

			// 3d matrix
			glm::vec3 cam_from = cast_object<glm::vec3>(current_camera_->owner->get_position());
			glm::vec3 cam_to = cast_object<glm::vec3>(current_camera_->owner->get_position() + current_camera_->owner->get_rotation().forward());
			cam_from.y *= -1;
			cam_to.y *= -1;
			glm::mat4 view = glm::lookAt(
                cam_from,
                cam_to,
                glm::vec3(0.0f, 0.0f, 1.0f)
                ) * glm::mat4 (
				    1.0, 0.0, 0.0, 0.0,
				    0.0, -1.0, 0.0, 0.0,
				    0.0, 0.0, 1.0, 0.0,
				    0.0, 0.0, 0.0, 1.0
			    );

			glm::mat4 proj = glm::perspective(current_camera_->fov, (float) width / (float) height, 0.01f, 1000.0f);

			glm::mat4 vp = proj * view;

			// UI matrix
			glm::mat4 ui_vp = glm::ortho(0.0f, (float)width, (float)-height, 0.0f, -1000.0f, 0.0001f);
			
			is_render_stage_ = true;
			renderer_->render(vp);
			glClear(GL_DEPTH_BUFFER_BIT);
			ui_renderer_->render(ui_vp);
			is_render_stage_ = false;
		}
 
		glfwSwapBuffers(window_);
		glfwPollEvents();

		last_delta_time = static_cast<float>(glfwGetTime() - tick_start);
	}

	Texture::print_usage_dump();

	close_world();
}

void Game::cleanup()
{
	verbose("Game", "Cleaning up...");
	
	renderer_->cleanup();
	ui_renderer_->cleanup();

	for (auto& kvp : shaders_)
	{
		kvp.second->cleanup();
	}
	
	shaders_.clear();

	Texture::unload_all_static();

	meshes_.clear();
}

void Game::init_game()
{
	info_ = GameInfo();

	info_.title = "Untitled Game";
	info_.physics_tick = 1.0f / 60.0f;

	init_game_info(info_);
}

void Game::set_app_path(const Path& new_app_path)
{
	app_path_ = new_app_path;
	app_path_set_ = true;
}

void Game::error_callback(int error, const char* description)
{
	print_error("OpenGL", "%i - %s", error, description);
}

void Game::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	else
	{
		if (instance_->current_controllable_)
		{
			if (action == GLFW_PRESS)
			{
				instance_->current_controllable_->key_down(key);
			}
			else if (action == GLFW_RELEASE)
			{
				instance_->current_controllable_->key_up(key);
			}
		}
	}
}

void Game::mouse_button_callback(class GLFWwindow* window, int button, int action, int mods)
{
	if (instance_->current_controllable_)
	{
		if (action == GLFW_PRESS)
		{
			Shared<UIElement> ui_under_mouse;
			float pressed_z = 0.0f;
			instance_->ui_root_->detect_topmost_under_mouse(instance_->mouse_pos_, 0.0f, ui_under_mouse, pressed_z);

			if (ui_under_mouse)
			{
				ui_under_mouse->on_press();
				instance_->pressed_ui_ = ui_under_mouse;
			}
			else
			{
				instance_->current_controllable_->mouse_button_down(button);
			}
		}
		else if (action == GLFW_RELEASE)
		{
			if (auto released_ui = instance_->pressed_ui_.lock())
			{
				released_ui->on_release();
				instance_->pressed_ui_.reset();
			}
			else
			{
				instance_->current_controllable_->mouse_button_up(button);
			}
		}
	}
}

void Game::cursor_position_callback(class GLFWwindow* window, double x_pos, double y_pos)
{
	instance_->mouse_pos_ = { static_cast<float>(x_pos), static_cast<float>(y_pos) };
	if (!instance_->has_mouse_pos_)
	{
		instance_->last_mouse_pos_ = instance_->mouse_pos_;
		instance_->has_mouse_pos_ = true;
	}
}

void Game::window_size_callback(class GLFWwindow* window, int width, int height)
{
	instance_->ui_root_->set_size(Vector2(static_cast<float>(width), static_cast<float>(height)));
}
