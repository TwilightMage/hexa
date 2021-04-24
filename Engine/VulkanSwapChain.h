#pragma once

#include "framework.h"

#include <vulkan/vulkan.hpp>

typedef struct _SwapChainBuffers {
	vk::Image image;
	vk::ImageView view;
} SwapChainBuffer;

class VulkanSwapChain
{
private:
	vk::Instance instance;
	vk::Device device;
	vk::PhysicalDevice physicalDevice;
	vk::SurfaceKHR surface;

	// Function pointers
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR fpQueuePresentKHR;
public:
	vk::Format colorFormat;
	vk::ColorSpaceKHR colorSpace;
	vk::SwapchainKHR swapChain = nullptr;
	uint32_t imageCount;
	std::vector<vk::Image> images;
	std::vector<SwapChainBuffer> buffers;
	uint32_t queueNodeIndex = UINT32_MAX;

#if PLATFORM_WINDOWS
	void InitSurface(void* platformHandle, void* platformWindow);
#endif
	void Connect(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device);
	void Create(uint32_t* width, uint32_t* height, bool vsync = false);
	vk::Result AcquireNextImage(vk::Semaphore presentCompleteSemaphore, uint32_t* imageIndex);
	vk::Result QueuePresent(vk::Queue queue, uint32_t imageIndex, vk::Semaphore waitSemaphore = nullptr);
	void Cleanup();
};