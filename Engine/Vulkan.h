#pragma once

#include "framework.h"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#include "VulkanSwapChain.h"


class Vulkan
{
    friend class GameWindow;

public:
    bool Init();

private:
    struct {
        // Swap chain image presentation
        vk::Semaphore presentComplete;
        // Command buffer submission and execution
        vk::Semaphore renderComplete;
    } semaphores;

    struct {
        vk::Image image;
        vk::DeviceMemory mem;
        vk::ImageView view;
    } depthStencil;

    vk::Result CreateInstance();

    bool IsPhysicalDeviceSuitable(vk::PhysicalDevice device);
    int RatePhysicalDeviceSuitability(vk::PhysicalDevice device);

    void InitSwapchain(class GameWindow* window);
    void CreateCommandPool();
    void SetupSwapChain(class GameWindow* window);
    void CreateCommandBuffers();
    void CreateSynchronizationPrimitives();
    void SetupDepthStencil(class GameWindow* window);
    void SetupRenderPass();
    void CreatePipelineCache();
    void SetupFrameBuffer(class GameWindow* window);
    void DestroyCommandBuffers();
    void BuildCommandBuffers(class GameWindow* window);

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    vk::Instance instance;
    struct VulkanDevice* vulkanDevice;
    VulkanSwapChain swapChain;
    vk::Device device;
    vk::Queue queue;
    vk::Format depthFormat;
    vk::SubmitInfo submitInfo;
    vk::PipelineStageFlags submitPipelineStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::CommandPool cmdPool;
    std::vector<vk::CommandBuffer> drawCmdBuffers;
    std::vector<vk::Fence> waitFences;
    std::vector<vk::Framebuffer> frameBuffers;
    vk::RenderPass renderPass;
    vk::PipelineCache pipelineCache;
    vk::Pipeline pipeline;
    uint32_t currentBuffer = 0;

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    vk::DebugUtilsMessengerEXT validationMessenger;

    std::vector<const char*> supportedInstanceExtensions;
    std::vector<const char*> enabledInstanceExtensions;
    std::vector<const char*> enabledDeviceExtensions;
    std::vector<const char*> physicalDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    void* deviceCreatepNextChain = nullptr;
};
