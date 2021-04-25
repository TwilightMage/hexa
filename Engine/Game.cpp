#include "Game.h"

#include <map>
#include <GLFW/glfw3.h>
#include "linmath.h"

#include "Path.h"

Game* Game::instance = nullptr;
Path Game::appPath = Path();
bool Game::appPathSet = false;

Game::Game(int argc, char* argv[])
    : logStream(DateTime::Now(), argv[0])
{
	if (instance)
	{
		throw std::runtime_error("Only one game instance may exist");
	}
	else
	{
		instance = this;
	}

	for (int i = 0; i < argc; i++)
	{
		args.Add(argv[i]);
	}

	appPath = String(argv[0]);
}

void Game::Launch()
{
	Verbose("Game", "Launching...");

	if (!glfwInit())
	{
		PrintError("Initialization", "Failed to initialize glfw");
	}
	
	InitGame();
    SetupWindow();
    Prepare();
    RenderLoop();
}

const List<String>& Game::GetArgs() const
{
	return args;
}

Game* Game::GetInstance()
{
	return instance;
}

const GameInfo& Game::GetInfo() const
{
	return info;
}

void Game::NewLogRecord(ELogLevel level, const String& category, const String& message)
{
	static const char* levelNames[4] = { "Verbose", "Debug", "Warning", "Error" };
	static const char* levelColors[4] = { CONSOLE_WHITE, CONSOLE_CYAN, CONSOLE_YELLOW, CONSOLE_RED };

	logStreamMutex.lock();
	logStream << levelColors[static_cast<int>(level)] << "[" << DateTime::Now().ToString().c() << "] [" << levelNames[static_cast<int>(level)] << "] [" << category.c() << "] " << message.c() << CONSOLE_RESET << "\n";
	logStreamMutex.unlock();
}

bool Game::IsAppPathSet()
{
	return appPathSet;
}

const Path& Game::GetAppPath()
{
	return appPath;
}

uint Game::GetScreenWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

uint Game::GetScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

void Game::SetupWindow()
{
	window = glfwCreateWindow(width, height, GetInfo().title.c(), nullptr, nullptr);
	if (!window)
	{
		PrintError("Initialization", "Failed to create window");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}

void Game::Prepare()
{
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
}

void Game::RenderLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		mat4x4 m, p, mvp;
 
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;
 
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
 
		mat4x4_identity(m);
		mat4x4_rotate_Z(m, m, (float) glfwGetTime());
		mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		mat4x4_mul(mvp, p, m);
 
		//glUseProgram(program);
		//glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
 
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Game::Loop()
{
}

void Game::InitGame()
{
	info = GameInfo();

	info.title = "Untitled Game";

	InitGameInfo(info);
}

void Game::SetAppPath(const Path& newAppPath)
{
	appPath = newAppPath;
	appPathSet = true;
}
