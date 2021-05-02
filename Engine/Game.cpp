#include "Game.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>


#include "Camera.h"
#include "IControllable.h"
#include "Math.h"
#include "Mod.h"
#include "Path.h"
#include "Quaternion.h"
#include "Renderer.h"
#include "Shader.h"
#include "World.h"

Game* Game::instance_ = nullptr;

Game::Game(int argc, char* argv[])
    : log_stream_(DateTime::Now(), argv[0])
	, event_bus_(new EventBus())
	, renderer_(new Renderer)
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
		instance_->world_->on_close();
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
	instance_->log_stream_ << levelColors[static_cast<int>(level)] << "[" << DateTime::Now().ToString().c() << "] [" << levelNames[static_cast<int>(level)] << "] [" << category.c() << "] " << message.c() << CONSOLE_RESET << "\n";
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

Weak<Shader> Game::get_basic_shader()
{
	return instance_->basic_shader_;
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
	glfwSetKeyCallback(window_, key_callback);
	glfwSetCursorPosCallback(window_, cursor_position_callback);
}

void Game::render_loop()
{
	String opengl_version(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	
	if (auto mod = Mod::load("mods/ExampleMod/ExampleMod.dll"))
	{
		mod->init(event_bus_.get());
	}
	
	Shader::meta basic_shader_meta;
	basic_shader_meta.vertex_param_size = sizeof(Mesh::vertex);
	basic_shader_meta.vertex_params = {
		{"vPos", 0, 3, GL_FLOAT},
		{"vUV", sizeof(float) * 3, 2, GL_FLOAT},
		{"vCol", sizeof(float) * 5, 3, GL_FLOAT}
	};
	basic_shader_meta.uniform_params = {
		{"MVPs"}
	};
	basic_shader_ = Shader::compile(Path("resources/engine/shaders/basic"), basic_shader_meta, Shader::VERTEX | Shader::FRAGMENT);
	
	start();

	float last_delta_time = 0.0f;
	
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

			glm::vec3 cam_from = copy_as<glm::vec3>(current_camera_->owner->position);
			glm::vec3 cam_to = copy_as<glm::vec3>(current_camera_->owner->position + current_camera_->owner->rotation.forward());
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
			
			renderer_->render(vp);
		}
 
		glfwSwapBuffers(window_);
		glfwPollEvents();

		last_delta_time = static_cast<float>(glfwGetTime() - tick_start);
	}
}

void Game::init_game()
{
	info_ = GameInfo();

	info_.title = "Untitled Game";

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

void Game::cursor_position_callback(class GLFWwindow* window, double x_pos, double y_pos)
{
	instance_->mouse_pos_ = { static_cast<float>(x_pos), static_cast<float>(y_pos) };
	if (!instance_->has_mouse_pos_)
	{
		instance_->last_mouse_pos_ = instance_->mouse_pos_;
		instance_->has_mouse_pos_ = true;
	}
}
