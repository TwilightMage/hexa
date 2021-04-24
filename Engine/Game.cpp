#include "Game.h"

#include <map>


#include "GameWindow.h"
#include "Path.h"
#include "Vulkan.h"
#include "VulkanUtilities.h"

Game* Game::instance = nullptr;

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

	for (uint i = 0; i < static_cast<uint>(argc); i++)
	{
		args.Add(argv[i]);
	}

	Path::SetAppPath(String(argv[0]));
}

void Game::Launch()
{
	Verbose("Game", "Launching...");

	InitGame();
    InitVulkan();
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

bool Game::InitVulkan()
{
	vulkan = new Vulkan();
	return vulkan->Init();
}

bool Game::SetupWindow()
{
	window = new GameWindow();
	window->TickFunc.Bind(this, &Game::Loop);
	return window->Init();
}

void Game::Prepare()
{
	window->Show();
	window->BindVulkan(vulkan);
}

void Game::RenderLoop()
{
	window->StartLoop();
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
