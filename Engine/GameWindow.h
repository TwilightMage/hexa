#pragma once

#include "DateTime.h"
#include "Delegate.h"
#include "framework.h"

enum class EWindowType
{
    Windowed,
    Fullscreen
};

IMPLEMENT_ENUM(EWindowType)

class GameWindow
{
    friend class Vulkan;

public:
    bool Init();
    bool Show();

    void StartLoop();
    void Quit();

    void BindVulkan(class Vulkan* vulkan);

    Action<> TickFunc;

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void NextFrame();
    void Tick();
    void Draw();

    void PrepareFrame();
    void SubmitFrame();

    void WindowResize();

    static std::map<HWND, GameWindow*> instances;

    const wchar_t* windowClassName = L"GameWindow";
    const HINSTANCE hInstance = GetModuleHandle(nullptr);
    HWND hWnd = nullptr;
    bool isInLoop = false;

    EWindowType type = EWindowType::Windowed;

    uint destWidth;
    uint destHeight;
    uint width = 0;
    uint height = 0;
    bool prepared = false;
    bool resized = false;

    uint frameCounter = 0;
    uint lastFPS = 0;
    float fpsTimer = 0.0f;

    DateTime lastUpdateTime;
    float timeDelta = 0.0f;

    class Vulkan* vulkanBinding;
};
