﻿#include "Game.h"

#include <OGRE/Bites/OgreTrays.h>
#include <OGRE/Main/OgreEntity.h>
#include <OGRE/Main/OgreRenderWindow.h>
#include <OGRE/Main/OgreTextureManager.h>
#include <OGRE/RTShaderSystem/OgreShaderGenerator.h>
#include <reactphysics3d/reactphysics3d.h>
#include <soloud/soloud.h>

#include "Audio.h"
#include "AudioChannel.h"
#include "CameraComponent.h"
#include "File.h"
#include "IControllable.h"
#include "Logger.h"
#include "Mod.h"
#include "Path.h"
#include "Paths.h"
#include "SaveGame.h"
#include "Set.h"
#include "Settings.h"
#include "SpriteFont.h"
#include "TextureAtlas.h"
#include "World.h"
#include "ui/Image.h"
#include "ui/TextBlock.h"
#include "ui/UIInputElement.h"

Game* Game::instance_ = nullptr;

Game::Game(const String& name, int argc, char* argv[])
	: app_path_(argv[0])
	, OgreBites::ApplicationContext(name.c())
	, Module(String(argv[0]).substring(0, String(argv[0]).last_index_of_char("\\/")), name)
	, event_bus_(new EventBus())
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

	Logger::init(argv[0]);
}

Game::~Game()
{
	instance_ = nullptr;
}

void Game::launch()
{
	verbose("Game", "Launching...");

	initApp();
	
	init_game();
    //setup_window();
    //prepare();
    render_loop();
	closeApp();
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

void Game::use_camera(const Shared<CameraComponent>& camera)
{
	instance_->current_camera_ = camera;
	instance_->getRenderWindow()->addViewport(camera->ogre_camera_);
}

void Game::open_world(const Shared<World>& world)
{
	close_world();

	instance_->world_ = world;
	if (instance_->world_)
	{
		instance_->world_->init();
		instance_->shader_generator_->addSceneManager(instance_->world_->manager_);
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
		instance_->shader_generator_->removeSceneManager(instance_->world_->manager_);
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

const Path& Game::get_app_path()
{
	return instance_->app_path_;
}

const Shared<Texture>& Game::get_white_pixel()
{
	return *instance_->white_pixel_;
}

const Shared<SpriteFont>& Game::get_default_font()
{
	return instance_->default_font_;
}

const Shared<AudioChannel>& Game::get_general_channel()
{
	return instance_->general_channel_;
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
	//instance_->lock_mouse_ = true;
}

void Game::unlock_mouse()
{
	//instance_->lock_mouse_ = false;
}

void Game::hide_mouse()
{
	//instance_->ogre_ui_->hideCursor();
}

void Game::show_mouse()
{
	//instance_->ogre_ui_->showCursor();
}

void Game::set_cursor_texture(const Shared<Texture>& tex, uint hotspot_x, uint hotspot_y)
{
	/*if (instance_->cursor_)
	{
		glfwDestroyCursor(instance_->cursor_);
		instance_->cursor_ = nullptr;
	}

	if (tex == nullptr)
	{
		glfwSetCursor(instance_->window_, nullptr);
		return;
	}
	
	uint scale = Math::round(instance_->ui_scale_);

	Array2D<Color> cursor_pixels = Array2D<Color>(tex->get_width() * scale, tex->get_height() * scale);

	for (uint x = 0; x < cursor_pixels.get_size_x(); x++)
	{
		for (uint y = 0; y < cursor_pixels.get_size_y(); y++)
		{
			cursor_pixels.at(x, y) = tex->get_pixel(x / scale, y / scale);
		}
	}

	uint len = cursor_pixels.get_size_x() * cursor_pixels.get_size_y() * 4;
	byte* pixels = new byte[len];
	memcpy(pixels, cursor_pixels.to_list().get_data(), len);
 
	GLFWimage image;
	image.width = cursor_pixels.get_size_x();
	image.height = cursor_pixels.get_size_y();
	image.pixels = pixels;
 
	instance_->cursor_ = glfwCreateCursor(&image, hotspot_x * scale, hotspot_y * scale);
	glfwSetCursor(instance_->window_, instance_->cursor_);

	delete pixels;*/
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

void Game::on_add_resource_directories(Set<String>& local, Set<String>& global)
{
	local = {
		"textures/actions"
	};	
	
	global = {
		"textures/ui",
		"textures/tiles",
		"textures/complex_tiles",
		"textures/characters",
		"meshes/characters",
		"meshes/items",
		"meshes/tiles",
		"audio/ambient",
		"audio/effects",
		"audio/music"
	};
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

bool Game::windowClosing(Ogre::RenderWindow* rw)
{
	close = true;
	return true;
}

void Game::setup()
{
	OgreBites::ApplicationContext::setup();

	addInputListener(this);

	OgreBites::WindowEventUtilities::_addRenderWindow(getRenderWindow());
	OgreBites::WindowEventUtilities::addWindowEventListener(getRenderWindow(), this);

	ogre_ = getRoot();

	//ogre_ui_ = MakeShared<OgreBites::TrayManager>("UI", getRenderWindow(), this);

	shader_generator_ = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	reset_global_resources_directories();
	add_resource_directories();
}

void Game::render_loop()
{
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
				mod->add_resource_directories();
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

	register_resource_directories();
	{
		Ogre::Image pixels(Ogre::PF_R5G6B5, 1, 1);
		pixels.setColourAt(Ogre::ColourValue::White, 0, 0, 0);

		Ogre::TextureManager::getSingleton().loadImage("White", get_module_name().c(), pixels);
	}
	for (const auto& mod : mods_)
	{
		mod->register_resource_directories();
	}

	Ogre::ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();
	
	// Textures
	white_pixel_ = MakeShared<Texture>("White Pixel", 1, 1, List<Color>::of(Color::white()));

	// Fonts
	default_font_ = SpriteFont::load_fnt(RESOURCES_FONTS + "arial.fnt");

	// Audio channels
	general_channel_ = AudioChannel::create();
	general_channel_->set_volume(settings_->audio_general);
	
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

	soloud_->set3dListenerUp(0, 0, 1);

	const auto start_time = std::chrono::system_clock::now();

	auto tick_start = start_time;
	
	while (!close)
	{		
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tick_start).count() / 1000.0f < 1.0f / settings_->fps_limit);
		const float tick_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tick_start).count() / 1000.0f;
		tick_start = std::chrono::system_clock::now();

		time_ += tick_time;
		
		main_thread_calls_mutex_.lock();
		for (auto& func : main_thread_calls_)
		{
			func();
		}
		main_thread_calls_.clear();
		main_thread_calls_mutex_.unlock();

		mouse_delta_ = mouse_pos_ - last_mouse_pos_;
		if (lock_mouse_)
		{
			//last_mouse_pos_ = mouse_pos_ = Vector2(static_cast<float>(width / 2), static_cast<float>(height / 2));
			//glfwSetCursorPos(window_, width / 2, height / 2);
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

		if (world_)
		{
			// ticking
			if (tick_time > 0.0f)
			{
				tick(tick_time);
				world_->tick(tick_time);
			}

			if (current_camera_)
			{
				auto cam_from = current_camera_->get_owner()->get_location();
				auto cam_to = current_camera_->get_owner()->get_location() + current_camera_->get_owner()->get_rotation().forward();
				
				// audio
				soloud_->set3dListenerPosition(cam_from.x, cam_from.y, cam_from.z);
				soloud_->set3dListenerAt(cam_to.x, cam_to.y, cam_to.z);

				getRoot()->renderOneFrame();
				
				/*// 3d matrix
				cam_from.y *= -1;
				cam_to.y *= -1;

				auto view = Matrix4x4::look_at(cam_from, cam_to) * Matrix4x4(
						1.0, 0.0, 0.0, 0.0,
						0.0, -1.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						0.0, 0.0, 0.0, 1.0
					);

				auto proj_3d = Matrix4x4::perspective(latest_camera_info_.fov, static_cast<float>(width) / static_cast<float>(height), 0.01f, 1000.0f);

				un_projected_mouse_ = Matrix4x4::un_project(Vector2(mouse_pos_.x, mouse_pos_.y), Vector2(static_cast<float>(width), static_cast<float>(height)), Matrix4x4().translate(latest_camera_info_.position), view, proj_3d);
				
				// UI matrix
				auto proj_ui = Matrix4x4::ortho(0.0f, static_cast<float>(width), static_cast<float>(-height), 0.0f, -1000.0f, 0.0001f);

				// rendering
				Material::RenderData render_data = {world_, view, proj_3d, proj_ui };

				auto view_shadow_sun = Matrix4x4::look_at(cam_from + -world_->sun_angle.forward() * 500.0f, cam_from) * Matrix4x4(
						1.0, 0.0, 0.0, 0.0,
						0.0, 1.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						0.0, 0.0, 0.0, 1.0
					);
				auto proj_shadow_sun = Matrix4x4::ortho(-10, 10, 10, -10, 0.01f, 1000.0f);
				Material::RenderData render_data_shadow = {world_, view_shadow_sun, proj_shadow_sun, Matrix4x4() };

				draw_call_counter_.clear();
			
				is_render_stage_ = true;
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);

				glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
				glBindFramebuffer(GL_FRAMEBUFFER, sun_shadow_fbo);
				glClear(GL_DEPTH_BUFFER_BIT);
				for (auto& material_list : materials_)
				{
					for (auto& material : material_list->value)
					{
						if (auto material3d = cast<Material3D>(material))
						{
							if (material3d->cast_shadows)
							{
								material->render(render_data_shadow);
							}
						}
					}
				}

				glViewport(0, 0, width, height);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				for (auto& material_list : materials_)
				{
					glClear(GL_DEPTH_BUFFER_BIT);
					for (auto& material : material_list->value)
					{
						material->render(render_data);
					}
				}
				is_render_stage_ = false;*/
			}
		}
	}

	Texture::print_usage_dump();

	close_world();
}

void Game::cleanup()
{
	verbose("Game", "Cleaning up...");

	unloading_stage();

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
	const Path settings_path = "settings.json";
	JSON settings_json;
	if (settings_path.exists())
	{
		settings_json = JSON::parse(File::read_file(settings_path));
	}
	settings_->read_settings(settings_json);
	//settings_->write_settings(settings_json);
	//File::write_file(settings_path, settings_json.to_string());

	soloud_->init();
}

bool Game::keyPressed(const OgreBites::KeyboardEvent& evt)
{
	if (instance_)
	{
		if (evt.repeat)
		{
			if (instance_->ui_input_element_)
			{
				instance_->ui_input_element_->key_hold(evt.keysym.sym);
			}
		}
		else
		{
			if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
			{
				close = true;
			}
			else if (instance_->ui_input_element_)
			{
				instance_->ui_input_element_->key_down(evt.keysym.sym);
			}
			else if (instance_->current_controllable_)
			{
				instance_->current_controllable_->key_down(evt.keysym.sym);
			}
		}
	}

	return true;
}

bool Game::keyReleased(const OgreBites::KeyboardEvent& evt)
{
	if (instance_)
	{
		if (instance_->ui_input_element_)
		{
			instance_->ui_input_element_->key_up(evt.keysym.sym);
		}
		else if (instance_->current_controllable_)
		{
			instance_->current_controllable_->key_up(evt.keysym.sym);
		}
	}

	return true;
}

bool Game::textInput(const OgreBites::TextInputEvent& evt)
{
	if (instance_ && instance_->ui_input_element_)
	{
		instance_->ui_input_element_->text_input(evt.chars[0]);
	}

	return true;
}

bool Game::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
	if (instance_ && instance_->current_controllable_)
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
				
			instance_->current_controllable_->mouse_button_down(evt.type);
		}
	}
	
	return true;
}

bool Game::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
	if (instance_->current_controllable_)
	{
		if (auto released_ui = instance_->pressed_ui_.lock())
		{
			released_ui->on_release();
			released_ui->is_pressed_ = false;
			instance_->pressed_ui_.reset();
		}
		else
		{
			instance_->current_controllable_->mouse_button_up(evt.type);
		}
	}
	
	return true;
}

bool Game::axisMoved(const OgreBites::AxisEvent& evt)
{
	if (instance_->ui_input_element_ == nullptr && instance_->current_controllable_)
	{
		instance_->current_controllable_->scroll(Vector2(0, evt.value));
	}

	return true;
}

bool Game::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
	instance_->mouse_pos_ = { static_cast<float>(evt.x), static_cast<float>(evt.y) };
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

	return true;
}

void Game::windowResized(Ogre::RenderWindow* rw)
{
	instance_->ui_root_->set_size(Vector2(static_cast<float>(rw->getWidth()) / get_ui_scale(), static_cast<float>(rw->getHeight()) / get_ui_scale()));
}
