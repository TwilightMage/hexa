#include "GameWindow.h"

#include "Game.h"
#include "String.h"
#include "Vulkan.h"
#include "VulkanUtilities.h"

std::map<HWND, GameWindow*> GameWindow::instances = std::map<HWND, GameWindow*>();

bool GameWindow::Init()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = windowClassName;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		PrintError("Window", "Failed to register window");
		return false;
	}

	width = 800;
	height = 600;

	return true;
}

bool GameWindow::Show()
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (type == EWindowType::Fullscreen)
	{
		if (width != static_cast<uint>(screenWidth) || height != static_cast<uint>(screenHeight))
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = width;
			dmScreenSettings.dmPelsHeight = height;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				if (MessageBox(nullptr, L"Fullscreen Mode not supported!\n Switch to window mode?", L"Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					type = EWindowType::Windowed;
				}
				else
				{
					return false;
				}
			}

			screenWidth = width;
			screenHeight = height;
		}
	}

	DWORD dwExStyle;
	DWORD dwStyle;

	if (type == EWindowType::Fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = type == EWindowType::Fullscreen ? static_cast<long>(screenWidth) : static_cast<long>(width);
	windowRect.bottom = type == EWindowType::Fullscreen ? static_cast<long>(screenHeight) : static_cast<long>(height);

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	hWnd = CreateWindowEx(0, windowClassName, Game::GetInstance()->GetInfo().title.wc(), dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);

	if (type == EWindowType::Windowed)
	{
		// Center on screen
        const uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
        const uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
		SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	if (hWnd == nullptr)
	{
		PrintError("Window", "Failed to create window");
		return false;
	}

	instances[hWnd] = this;

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	return true;
}

void GameWindow::StartLoop()
{
	if (isInLoop) return;
	isInLoop = true;

	destWidth = width;
	destHeight = height;
#if PLATFORM_WINDOWS
	MSG msg;
	bool quitMessageReceived = false;
	while (!quitMessageReceived)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				quitMessageReceived = true;
				break;
			}
		}

		if (prepared && !IsIconic(hWnd))
		{
			NextFrame();
		}

		lastUpdateTime = DateTime::Now();
	}
#endif
	// Flush device to make sure all resources can be freed
	if (vulkanBinding->device != VK_NULL_HANDLE)
	{
		vulkanBinding->device.waitIdle();
	}
}

void GameWindow::Quit()
{
	//endLoop = true;
}

void GameWindow::BindVulkan(Vulkan* vulkan)
{
	if (vulkanBinding == nullptr)
	{
		/*if (vulkanDevice->enableDebugMarkers) {
		vks::debugmarker::setup(device);
	    }*/
		vulkan->InitSwapchain(this);
		vulkan->CreateCommandPool();
		vulkan->SetupSwapChain(this);
		vulkan->CreateCommandBuffers();
		vulkan->CreateSynchronizationPrimitives();
		vulkan->SetupDepthStencil(this);
		vulkan->SetupRenderPass();
		vulkan->CreatePipelineCache();
		vulkan->SetupFrameBuffer(this);
		/*settings.overlay = settings.overlay && (!benchmark.active);
		if (settings.overlay) {
			UIOverlay.device = vulkanDevice;
			UIOverlay.queue = queue;
			UIOverlay.shaders = {
				loadShader(getShadersPath() + "base/uioverlay.vert.spv", VK_SHADER_STAGE_VERTEX_BIT),
				loadShader(getShadersPath() + "base/uioverlay.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT),
			};
			UIOverlay.prepareResources();
			UIOverlay.preparePipeline(pipelineCache, renderPass);
		}*/

		vulkanBinding = vulkan;
	}
}

LRESULT GameWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void GameWindow::NextFrame()
{
	/*if (viewUpdated)
	{
		viewUpdated = false;
		viewChanged();
	}*/

	Tick();

	/*frameTimer = (float)tDiff / 1000.0f;
	camera.update(frameTimer);
	if (camera.moving())
	{
		viewUpdated = true;
	}
	// Convert to clamped timer value
	if (!paused)
	{
		timer += timerSpeed * frameTimer;
		if (timer > 1.0)
		{
			timer -= 1.0f;
		}
	}*/
	timeDelta = (DateTime::Now() - lastUpdateTime).GetTotalSeconds();
	fpsTimer += timeDelta;
	if (fpsTimer > 1)
	{
		lastFPS = frameCounter;
	}
	frameCounter++;

	SetWindowText(hWnd, StringFormat("%s - %i FPS", Game::GetInstance()->GetInfo().title.c(), lastFPS).wc());

	// TODO: Cap UI overlay update rates
	//updateOverlay();
}

void GameWindow::Tick()
{
	if (!prepared) return;

	Draw();

	TickFunc();

	/*if (animate) {
		cubes[0].rotation.x += 2.5f * frameTimer;
		if (cubes[0].rotation.x > 360.0f)
			cubes[0].rotation.x -= 360.0f;
		cubes[1].rotation.y += 2.0f * frameTimer;
		if (cubes[1].rotation.x > 360.0f)
			cubes[1].rotation.x -= 360.0f;
	}*/

	//UpdateUniformBuffers();

	/*
	cubes[0].matrices.model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
	cubes[1].matrices.model = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.5f, 0.0f));

	for (auto& cube : cubes) {
		cube.matrices.projection = camera.matrices.perspective;
		cube.matrices.view = camera.matrices.view;
		cube.matrices.model = glm::rotate(cube.matrices.model, glm::radians(cube.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		cube.matrices.model = glm::rotate(cube.matrices.model, glm::radians(cube.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		cube.matrices.model = glm::rotate(cube.matrices.model, glm::radians(cube.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		cube.matrices.model = glm::scale(cube.matrices.model, glm::vec3(0.25f));
		memcpy(cube.uniformBuffer.mapped, &cube.matrices, sizeof(cube.matrices));
	}
	*/
}

void GameWindow::Draw()
{
	PrepareFrame();
	vulkanBinding->submitInfo.commandBufferCount = 1;
	vulkanBinding->submitInfo.pCommandBuffers = &vulkanBinding->drawCmdBuffers[vulkanBinding->currentBuffer];
	CHECK_VULKAN_RESULT(vulkanBinding->queue.submit(1, &vulkanBinding->submitInfo, nullptr));
	SubmitFrame();
}

void GameWindow::PrepareFrame()
{
	// Acquire the next image from the swap chain
	vk::Result result = vulkanBinding->swapChain.AcquireNextImage(vulkanBinding->semaphores.presentComplete, &vulkanBinding->currentBuffer);
	// Recreate the swapchain if it's no longer compatible with the surface (OUT_OF_DATE) or no longer optimal for presentation (SUBOPTIMAL)
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
	{
		WindowResize();
	}
	else
	{
		CHECK_VULKAN_RESULT(result);
	}
}

void GameWindow::SubmitFrame()
{
	vk::Result result = vulkanBinding->swapChain.QueuePresent(vulkanBinding->queue, vulkanBinding->currentBuffer, vulkanBinding->semaphores.renderComplete);
	if (!(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR))
	{
		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			// Swap chain is no longer compatible with the surface and needs to be recreated
			WindowResize();
			return;
		}
		else
		{
			CHECK_VULKAN_RESULT(result);
		}
	}
	vulkanBinding->queue.waitIdle();
}

void GameWindow::WindowResize()
{
	if (!prepared) return;

	prepared = false;
	resized = true;

	// Ensure all operations on the device have been finished before destroying resources
	vulkanBinding->device.waitIdle();

	// Recreate swap chain
	width = destWidth;
	height = destHeight;

	vulkanBinding->SetupSwapChain(this);

	// Recreate the frame buffers
	vulkanBinding->device.destroyImageView(vulkanBinding->depthStencil.view, nullptr);
	vulkanBinding->device.destroyImage(vulkanBinding->depthStencil.image, nullptr);
	vulkanBinding->device.freeMemory(vulkanBinding->depthStencil.mem, nullptr);

	vulkanBinding->SetupDepthStencil(this);

    for (uint32_t i = 0; i < vulkanBinding->frameBuffers.size(); i++) {
		vulkanBinding->device.destroyFramebuffer(vulkanBinding->frameBuffers[i], nullptr);
	}
	vulkanBinding->SetupFrameBuffer(this);

	if (width > 0.0f && height > 0.0f)
	{
		/*if (settings.overlay) {
			UIOverlay.resize(width, height);
		}*/
	}

	// Command buffers need to be recreated as they may store
	// references to the recreated frame buffer
	vulkanBinding->DestroyCommandBuffers();
	vulkanBinding->CreateCommandBuffers();
	vulkanBinding->BuildCommandBuffers();

	vkDeviceWaitIdle(device);

	if ((width > 0.0f) && (height > 0.0f)) {
		camera.updateAspectRatio((float)width / (float)height);
	}

	// Notify derived class
	windowResized();
	viewChanged();

	prepared = true;
}
