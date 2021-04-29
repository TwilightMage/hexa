#include "Game.h"

#include <map>
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

void Game::possess(IControllable* controllable)
{
	current_controllable_ = controllable;
}

void Game::use_camera(Camera* camera)
{
	current_camera_ = camera;
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

	world_ = new World();
	world_->start();

	world_->spawn<DemoMeshEntity>(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
	world_->spawn<DemoMeshEntity>(glm::vec3(0.0f, 0.0f, 1.0f), glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(90.0f))));
	world_->spawn<DemoMeshEntity>(glm::vec3(0.0f, 0.0f, 2.0f), glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(180.0f))));
	world_->spawn<DemoMeshEntity>(glm::vec3(0.0f, 0.0f, 3.0f), glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(270.0f))));

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
			
			for (const auto& shader_meshes : render_database)
            {
            	glUseProgram(shader_meshes.value.gl_shader_id);
            	glBindBuffer(GL_ARRAY_BUFFER, shader_meshes.value.gl_vertex_buffer_id);
				shader_meshes.key->map_params();

                const uint mvp_array_size = std::min(shader_meshes.value.objects_count, shader_render_data::objects_count_limit);
				glm::mat4* mvp_array = new glm::mat4[mvp_array_size];
				{
            		uint i = 0;
            		for (const auto& mesh_objects : shader_meshes.value)
            		{
            			for (auto object : mesh_objects.value)
            			{
            				mvp_array[i++] = vp * get_model_matrix(object.get());
            				if (i == mvp_array_size) break;
            			}
            			if (i == mvp_array_size) break;
            		}
				}
				glUniformMatrix4fv(0, mvp_array_size, GL_FALSE, glm::value_ptr(mvp_array[0]));
				delete mvp_array;
				
            	for (const auto& mesh_objects : shader_meshes.value)
            	{           		
            		glDrawArraysInstanced(GL_TRIANGLES, mesh_objects.value.vertex_buffer_offset, mesh_objects.value.size_in_vertex_buffer, mesh_objects.value.Length());
            	}
            }
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

void Game::key_callback(class GLFWwindow* window, int key, int scancode, int action, int mods)
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
