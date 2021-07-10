#include "Game.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <reactphysics3d/reactphysics3d.h>

#include "Audio.h"
#include "AudioChannel.h"
#include "ICamera.h"
#include "IControllable.h"
#include "Logger.h"
#include "Mod.h"
#include "Path.h"
#include "Paths.h"
#include "Material.h"
#include "Material3D.h"
#include "MaterialUI.h"
#include "SaveGame.h"
#include "Set.h"
#include "Settings.h"
#include "Shader.h"
#include "SpriteFont.h"
#include "TextureAtlas.h"
#include "World.h"
#include "HexaGame/HexaSettings.h"
#include "ui/Image.h"
#include "ui/TextBlock.h"
#include "ui/UIInputElement.h"

Game* Game::instance_ = nullptr;

Game::Game(int argc, char* argv[])
    : event_bus_(new EventBus())
	, physics_(new reactphysics3d::PhysicsCommon)
	, soloud_(new SoLoud::Soloud)
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
		args_.add(argv[i]);
	}

	set_app_path(String(argv[0]));

	Logger::init(argv[0]);
}

Game::~Game()
{
	instance_ = nullptr;
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

void Game::possess(const Shared<IControllable>& controllable)
{
	if (instance_->current_controllable_)
	{
		instance_->current_controllable_->on_unpossess();
	}
	instance_->current_controllable_ = controllable;
	instance_->current_controllable_->on_possess();
}

void Game::focus_ui(const Shared<UIInputElement>& ui_input_reciever)
{
	if (instance_->ui_input_element_)
	{
		instance_->ui_input_element_->on_unfocus();
	}
	
	instance_->ui_input_element_ = ui_input_reciever;

	if (instance_->ui_input_element_)
	{
		instance_->ui_input_element_->on_focus();
	}
}

void Game::use_camera(const Shared<ICamera>& camera)
{
	instance_->current_camera_ = camera;
}

void Game::open_world(const Shared<World>& world)
{
	close_world();

	instance_->world_ = world;
	if (world)
	{
		instance_->world_->start();
		instance_->event_bus_->world_opened(world);
	}
	else
	{
		print_warning("Game", "Attempt to open nullptr world");
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

const Shared<World>& Game::get_world()
{
	return instance_->world_;
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

const Shared<Settings>& Game::get_settings()
{
	return instance_->settings_;
}

const Shared<SaveGame>& Game::get_save_game()
{
	return instance_->save_game_;
}

const Shared<EventBus>& Game::get_event_bus()
{
	return instance_->event_bus_;
}

void Game::call_on_main_thread(std::function<void()> func)
{
	instance_->main_thread_calls_mutex_.lock();
	instance_->main_thread_calls_.add(func);
	instance_->main_thread_calls_mutex_.unlock();
}

bool Game::is_app_path_set()
{
	return instance_->app_path_set_;
}

const Path& Game::get_app_path()
{
	return instance_->app_path_;
}

Shared<Shader> Game::get_basic_shader()
{
	return instance_->basic_3d_shader_;
}

Shared<Shader> Game::get_basic_ui_shader()
{
	return instance_->basic_ui_shader_;
}

Shared<Material3D> Game::get_basic_material_3d()
{
	return instance_->basic_material_3d_;
}

Shared<MaterialUI> Game::get_basic_material_ui()
{
	return instance_->basic_material_ui_;
}

Shared<Texture> Game::get_white_pixel()
{
	return *instance_->white_pixel_;
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

void Game::add_ui(const Shared<UIElement>& ui)
{
	instance_->ui_root_->add_child(ui);
}

float Game::get_ui_scale()
{
	return instance_->ui_scale_;
}

Vector3 Game::get_un_projected_mouse()
{
	return instance_->un_projected_mouse_;
}

CameraInfo& Game::get_camera_info()
{
	return instance_->latest_camera_info_;
}

float Game::get_time()
{
	return instance_->time_;
}

bool Game::is_loading_stage()
{
	return instance_->is_loading_stage_;
}

bool Game::is_render_stage()
{
	return instance_->is_render_stage_;
}

Shared<Settings> Game::generate_settings_object()
{
	return MakeShared<Settings>();
}

Shared<SaveGame> Game::generate_save_game_object(const String& profile_name)
{
	return MakeShared<SaveGame>(profile_name);
}

Shared<EventBus> Game::generate_event_bus_object()
{
	return MakeShared<EventBus>();
}

void Game::loading_stage()
{
}

void Game::start()
{
}

void Game::tick(float delta_time)
{
}

void Game::unloading_stage()
{
}

void Game::setup_window()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	ui_root_->set_size(Vector2(800 / get_ui_scale(), 600 / get_ui_scale()));
	ui_root_->mouse_detection_ = false;
	ui_root_->constructed_ = true;
	ui_root_->is_in_hierarchy_ = true;
	
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
	glfwSwapInterval(0);
	
	glfwSetErrorCallback(error_callback);
	glfwSetWindowSizeLimits(window_, 800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetKeyCallback(window_, key_callback);
	glfwSetCharCallback(window_, character_callback);
	glfwSetMouseButtonCallback(window_, mouse_button_callback);
	glfwSetScrollCallback(window_, scroll_callback);
	glfwSetCursorPosCallback(window_, cursor_position_callback);
	glfwSetWindowSizeCallback(window_, window_size_callback);
}

void Game::render_loop()
{	
	const String GPU_name(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	verbose("OpenGL", "Active GPU: %s", GPU_name.c());
	glfwSetWindowTitle(window_, (get_info().title + " - " + GPU_name).c());
	
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
				mods_.add(mod);
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

	// Shaders
	basic_3d_shader_ = Shader::compile("basic 3d", {
		RESOURCES_ENGINE_SHADERS + "basic_3d.vert",
		RESOURCES_ENGINE_SHADERS + "basic_3d.frag"
	});

	basic_ui_shader_ = Shader::compile("basic ui", {
		RESOURCES_ENGINE_SHADERS + "basic_ui.vert",
		RESOURCES_ENGINE_SHADERS + "basic_ui.frag"
	});

	// Materials
	basic_material_3d_ = MakeShared<Material3D>();
	basic_material_3d_->init(basic_3d_shader_, 0);
	
	basic_material_ui_ = MakeShared<MaterialUI>();
	basic_material_ui_->init(basic_ui_shader_, 1);

	// Textures
	white_pixel_ = MakeShared<Texture>("White Pixel", 1, 1, List<Color>::of(Color::white()));

	// Fonts
	default_font_ = SpriteFont::load_fnt(RESOURCES_ENGINE_FONTS + "arial.fnt");
	
	loading_stage();
	
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
			mod->on_loaded(event_bus_);
			verbose("Mod Loader", "Post-loaded mod %s", mod->info_.name.c());
		}
		catch (std::runtime_error err)
		{
			print_error("Mod Loader", "Failed to post-load mod %s: %s", mod->info_.name.c(), err.what());
			return;
		}
	}

	save_game_ = generate_save_game_object("soft fur dragon");

	verbose("Game", "Starting...");
	start();

	float fps_delta_time_stack = 0;
	uint fps_count = 0;
	uint fps_last_count = 0;
	
	auto fps_display = MakeShared<TextBlock>();
	fps_display->set_z(10);
	add_ui(fps_display);

	auto dcc_display = MakeShared<TextBlock>();
	dcc_display->set_z(10);
	dcc_display->set_position(Vector2(0, 50));
	add_ui(dcc_display);

	float tick_start = glfwGetTime();

	while (!glfwWindowShouldClose(window_))
	{
		while (glfwGetTime() - tick_start < 1.0f / settings_->get_fps_limit());
		const float tick_time = glfwGetTime() - tick_start;
		tick_start = glfwGetTime();

		time_ += tick_time;
		
		main_thread_calls_mutex_.lock();
		for (auto& func : main_thread_calls_)
		{
			func();
		}
		main_thread_calls_.clear();
		main_thread_calls_mutex_.unlock();
		
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

		fps_delta_time_stack += tick_time;
		fps_count++;
		if (fps_delta_time_stack >= 1)
		{
			fps_delta_time_stack -= 1;
			fps_last_count = fps_count;
			fps_count = 0;
		}
		
		fps_display->set_text(String::format("FPS: %i", fps_last_count));

		String dcc_string = "--draw calls--";
		for (auto& dcc : draw_call_counter_)
		{
			dcc_string += "\n" + dcc->key + ": " + String::make(dcc->value);
		}
		dcc_display->set_text(dcc_string);

		if (world_)
		{
			basic_material_3d_->set_param_value("ambient_light", world_->ambient_color.to_vector3() * world_->ambient_intensity);
			basic_material_3d_->set_param_value("sun_light", world_->sun_color.to_vector3() * world_->sun_intensity);
			basic_material_3d_->set_param_value("sun_dir", -world_->sun_angle.forward());
			
			// ticking
			if (tick_time > 0.0f)
			{
				tick(tick_time);
				world_->tick(tick_time);
			}

			if (current_camera_ && width > 0 && height > 0)
			{
				// 3d matrix
				latest_camera_info_ = current_camera_->get_camera_info();
				auto cam_from = latest_camera_info_.position;
				auto cam_to = latest_camera_info_.position + latest_camera_info_.rotation.forward();
				cam_from.y *= -1;
				cam_to.y *= -1;

				auto view = Matrix4x4::look_at(cam_from, cam_to) * Matrix4x4(
						1.0, 0.0, 0.0, 0.0,
						0.0, -1.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						0.0, 0.0, 0.0, 1.0
					);

				auto proj = Matrix4x4::perspective(latest_camera_info_.fov, static_cast<float>(width) / static_cast<float>(height), 0.01f, 1000.0f);

				un_projected_mouse_ = Matrix4x4::un_project(Vector2(mouse_pos_.x, mouse_pos_.y), Vector2(static_cast<float>(width), static_cast<float>(height)), Matrix4x4().translate(latest_camera_info_.position), view, proj);
				
				// UI matrix
				auto ui_proj = Matrix4x4::ortho(0.0f, static_cast<float>(width), static_cast<float>(-height), 0.0f, -1000.0f, 0.0001f);

				// rendering
				Material::RenderData render_data = {world_, view, proj, ui_proj };

				draw_call_counter_.clear();
			
				is_render_stage_ = true;
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				for (auto& material_list : materials_)
				{
					glClear(GL_DEPTH_BUFFER_BIT);
					for (auto& material : material_list->value)
					{
						material->render(render_data);
					}
				}
				is_render_stage_ = false;
			}
		}
 
		glfwSwapBuffers(window_);
		glfwPollEvents();
	}

	Texture::print_usage_dump();

	close_world();
}

void Game::cleanup()
{
	verbose("Game", "Cleaning up...");

	unloading_stage();
	
	for (auto& material_list : materials_)
	{
		for (auto& material : material_list->value)
		{
			material->cleanup();
		}
	}

	for (auto& shader : shaders_)
	{
		shader.value->cleanup();
	}
	
	shaders_.clear();

	Texture::unload_all_static();

	meshes_.clear();

	soloud_->stopAll();
	
	for (auto& audio : audios_)
	{
		audio.value->sample_.reset();
	}

	for (auto& audio_channel : audio_channels_)
	{
		audio_channel->bus_.reset();
	}
	
	soloud_->deinit();
}

void Game::init_game()
{
	info_ = GameInfo();

	info_.title = "Untitled Game";

	init_game_info(info_);
	settings_ = generate_settings_object();

	soloud_->init();
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
	else if (instance_)
	{
		if (instance_->ui_input_element_)
		{
			if (action == GLFW_PRESS)
			{
				instance_->ui_input_element_->key_down(key);
			}
			else if (action == GLFW_RELEASE)
			{
				instance_->ui_input_element_->key_up(key);
			}
			else if (action == GLFW_REPEAT)
			{
				instance_->ui_input_element_->key_hold(key);
			}
		}
		else if (instance_->current_controllable_)
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

void Game::character_callback(class GLFWwindow* window, uint codepoint)
{
	if (instance_->ui_input_element_)
	{
		instance_->ui_input_element_->text_input(codepoint);
	}
}

void Game::mouse_button_callback(class GLFWwindow* window, int button, int action, int mods)
{
	if (instance_->current_controllable_)
	{
		if (action == GLFW_PRESS)
		{
			if (auto ui_under_mouse = instance_->ui_under_mouse_.lock())
			{
				if (instance_->ui_input_element_ && instance_->ui_input_element_ != ui_under_mouse)
				{
					instance_->ui_input_element_->on_unfocus();
					instance_->ui_input_element_ = nullptr;
				}

				auto inv_mat = ui_under_mouse->get_ui_matrix().inverse();
				auto q1 = inv_mat * ui_under_mouse->get_position();
				auto q2 = inv_mat * (ui_under_mouse->get_position() + ui_under_mouse->get_size());
				auto q = inv_mat * instance_->mouse_pos_ / instance_->ui_scale_;

				const auto rel_pos_aspect = (q - q1) / (q2 - q1);
				const auto rel_pos = rel_pos_aspect * ui_under_mouse->get_size();
				
				ui_under_mouse->on_press(rel_pos);
				ui_under_mouse->is_pressed_ = true;
				instance_->pressed_ui_ = ui_under_mouse;
			}
			else
			{
				if (instance_->ui_input_element_)
				{
					instance_->ui_input_element_->on_unfocus();
					instance_->ui_input_element_ = nullptr;
				}
				
				instance_->current_controllable_->mouse_button_down(button);
			}
		}
		else if (action == GLFW_RELEASE)
		{
			if (auto released_ui = instance_->pressed_ui_.lock())
			{
				released_ui->on_release();
				released_ui->is_pressed_ = false;
				instance_->pressed_ui_.reset();
			}
			else
			{
				instance_->current_controllable_->mouse_button_up(button);
			}
		}
	}
}

void Game::scroll_callback(class GLFWwindow* window, double xoffset, double yoffset)
{
	if (instance_->ui_input_element_ == nullptr && instance_->current_controllable_)
	{
		instance_->current_controllable_->scroll(Vector2(xoffset, yoffset));
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

	Shared<UIElement> ui_under_mouse;
	float pressed_z = 0.0f;
	instance_->ui_root_->detect_topmost_under_mouse(instance_->mouse_pos_, 0.0f, ui_under_mouse, pressed_z);

	auto current_ui_under_mouse = instance_->ui_under_mouse_.lock();

	if (current_ui_under_mouse != ui_under_mouse)
	{
		if (current_ui_under_mouse)
		{
			current_ui_under_mouse->on_mouse_leave();
			current_ui_under_mouse->is_mouse_over_ = false;
		}
		if (ui_under_mouse)
		{
			ui_under_mouse->on_mouse_enter();
			ui_under_mouse->is_mouse_over_ = true;
		}
			
		instance_->ui_under_mouse_ = ui_under_mouse;
	}
}

void Game::window_size_callback(class GLFWwindow* window, int width, int height)
{
	instance_->ui_root_->set_size(Vector2(static_cast<float>(width) / get_ui_scale(), static_cast<float>(height) / get_ui_scale()));
}
