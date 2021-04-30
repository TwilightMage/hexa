﻿#include "Game.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>


#include "Camera.h"
#include "DebugPlayer.h"
#include "DemoMeshEntity.h"
#include "linmath.h"
#include "Path.h"
#include "Renderer.h"
#include "Shader.h"
#include "World.h"

Game* Game::instance_ = nullptr;
Path Game::app_path_ = Path();
bool Game::app_path_set_ = false;

Game::Game(int argc, char* argv[])
    : log_stream_(DateTime::Now(), argv[0])
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
	current_controllable_ = controllable.lock();
}

void Game::use_camera(const Weak<Camera>& camera)
{
	current_camera_ = camera.lock();
}

const List<String>& Game::get_args() const
{
	return args_;
}

Game* Game::get_instance()
{
	return instance_;
}

const GameInfo& Game::get_info() const
{
	return info_;
}

void Game::new_log_record(ELogLevel level, const String& category, const String& message)
{
	static const char* levelNames[4] = { "Verbose", "Debug", "Warning", "Error" };
	static const char* levelColors[4] = { CONSOLE_WHITE, CONSOLE_CYAN, CONSOLE_YELLOW, CONSOLE_RED };

	log_stream_mutex_.lock();
	log_stream_ << levelColors[static_cast<int>(level)] << "[" << DateTime::Now().ToString().c() << "] [" << levelNames[static_cast<int>(level)] << "] [" << category.c() << "] " << message.c() << CONSOLE_RESET << "\n";
	log_stream_mutex_.unlock();
}

bool Game::is_app_path_set()
{
	return app_path_set_;
}

const Path& Game::get_app_path()
{
	return app_path_;
}

Weak<Shader> Game::get_basic_shader() const
{
	return basic_shader_;
}

uint Game::get_screen_width()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

uint Game::get_screen_height()
{
	return GetSystemMetrics(SM_CYSCREEN);
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
}

void Game::render_loop()
{
	String opengl_version(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	
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
	basic_shader_ = Shader::compile(Path("resources/engine/shaders/basic.frag"), Path("resources/engine/shaders/basic.vert"), basic_shader_meta);

	world_ = MakeShared<World>();
	world_->start();

	auto a = world_->spawn<DemoMeshEntity>(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
	auto b = world_->spawn<DemoMeshEntity>(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(90.0f))));
	auto c = world_->spawn<DemoMeshEntity>(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(180.0f))));
	auto d = world_->spawn<DemoMeshEntity>(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(270.0f))));

	//a->destroy();
	//b->destroy();
	//c->destroy();
	//d->destroy();
	
	auto player = world_->spawn<DebugPlayer>(glm::vec3(-5.0f, 0.0f, 2.0f), glm::quat(glm::vec3(0.0f, glm::radians(30.0f), 0.0f)));
	possess(player);
	use_camera(player->camera);

	float last_delta_time = 0.0f;
	
	while (!glfwWindowShouldClose(window_))
	{
		const auto tick_start = glfwGetTime();
		
		int width, height;
 
		glfwGetFramebufferSize(window_, &width, &height);
		
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		
		if (current_camera_ && current_camera_->owner)
		{
			if (last_delta_time > 0.0f)
			{
				world_->tick(last_delta_time);
			}

			glm::vec3 cam_from = current_camera_->owner->position;
			glm::vec3 cam_to = current_camera_->owner->position + current_camera_->owner->rotation * glm::vec3(1.0f, 0.0f, 0.0f);
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

			glm::mat4 proj = glm::perspective(current_camera_->fov, (float) width / (float) height, 0.0f, 10.0f);

			glm::mat4 vp = proj * view;
			
			renderer_.render(vp);
		}
 
		glfwSwapBuffers(window_);
		glfwPollEvents();

		last_delta_time = glfwGetTime() - tick_start;
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
