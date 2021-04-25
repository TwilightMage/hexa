#pragma once

#include <mutex>
#include <optional>


#include "GameInfo.h"
#include "List.h"
#include "LogStream.h"
#include "Path.h"
#include "String.h"

enum class ELogLevel
{
    Verbose,
    Debug,
    Warning,
    Error
};

EXTERN class EXPORT Game
{
public:
    Game(int argc, char* argv[]);

    void Launch();

    const List<String>& GetArgs() const;

    static Game* GetInstance();

    const GameInfo& GetInfo() const;

    void NewLogRecord(ELogLevel level, const String& category, const String& message);

    static bool IsAppPathSet();
    static const Path& GetAppPath();

    static uint GetScreenWidth();
    static uint GetScreenHeight();

protected:
    virtual void InitGameInfo(GameInfo& outInfo) = 0;

private:
    void SetupWindow();
    void Prepare();
    void RenderLoop();

    void Loop();

    void InitGame();

    static void SetAppPath(const Path& newAppPath);

    static Game* instance;

    List<String> args;

    GameInfo info;

    LogStream logStream;
    std::mutex logStreamMutex;
    
    static Path appPath;
    static bool appPathSet;

    class GLFWwindow* window;
    uint width = 800;
    uint height = 600;
};

template<typename... Args>
static void PrintDebug(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

	Game::GetInstance()->NewLogRecord(ELogLevel::Debug, category, buffer);
}

template<typename... Args>
static void PrintWarning(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Game::GetInstance()->NewLogRecord(ELogLevel::Warning, category, buffer);
}

template<typename... Args>
static void PrintError(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Game::GetInstance()->NewLogRecord(ELogLevel::Error, category, buffer);
}

template<typename... Args>
static void Verbose(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
	char* buffer = new char[size + 1];
	sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Game::GetInstance()->NewLogRecord(ELogLevel::Verbose, category, buffer);
}