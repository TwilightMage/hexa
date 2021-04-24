#include "Vulkan.h"

#include <map>

#include "Game.h"
#include "GameWindow.h"
#include "VulkanDevice.h"
#include "VulkanUtilities.h"

bool Vulkan::Init()
{
	vk::Result err;

	// Vulkan instance
	err = CreateInstance();
	if (err != vk::Result::eSuccess)
	{
		vku::tools::ExitFatal(StringFormat("Could not create Vulkan instance: %s", vku::tools::ErrorString(err)), err);
		return false;
	}

	// If requested, we enable the default validation layers for debugging
#if DEBUG
	vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
	vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));

	vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI;
	debugUtilsMessengerCI.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	debugUtilsMessengerCI.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
	debugUtilsMessengerCI.pfnUserCallback = VulkanValidationCallback;;
	CHECK_VULKAN_RESULT(static_cast<vk::Result>(vkCreateDebugUtilsMessengerEXT(static_cast<VkInstance>(instance), reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugUtilsMessengerCI), nullptr, reinterpret_cast<VkDebugUtilsMessengerEXT*>(&validationMessenger))));
#endif

	auto physicalDevices = instance.enumeratePhysicalDevices();

	assert(physicalDevices.size() > 0);

	// GPU selection

	// Select physical device to be used for the Vulkan example
	std::multimap<int, vk::PhysicalDevice> scoreMap;
	for (const auto& physicalDevice : physicalDevices)
	{
		int score = RatePhysicalDeviceSuitability(physicalDevice);
		scoreMap.insert(std::make_pair(score, physicalDevice));
	}

	vk::PhysicalDevice selectedPhysicalDevice = scoreMap.rbegin()->second;

	// GPU selection via command line argument
	int selectedDevice = -1;
	const auto& args = Game::GetInstance()->GetArgs();
	for (uint i = 0; i < args.Length(); i++)
	{
		// Select GPU
		if ((args[i] == std::string("-g")) || (args[i] == std::string("-gpu")))
		{
			for (uint j = 0; j < physicalDevices.size(); j++)
			{
				if (strcmp(physicalDevices[j].getProperties().deviceName, args[i + 1].c()) == 0)
				{
					selectedDevice = j;
					break;
				}
			}

			if (selectedDevice == -1)
			{
				PrintWarning("Vulkan", "Selected gpu is not valid, reverting to %s", scoreMap.rbegin()->second.getProperties().deviceName);
			}
			else
			{
				selectedPhysicalDevice = physicalDevices[selectedDevice];
				Verbose("Vulkan", "Selected gpu %s", selectedPhysicalDevice.getProperties().deviceName);
			}

			break;
		}

		// List available GPUs
		if (args[i] == std::string("-listgpus"))
		{
			if (physicalDevices.size() == 0)
			{
				PrintError("Vulkan", "No Vulkan devices found!");
			}
			else
			{
				// Enumerate devices
				List<String> physicalDeviceDescriptions;
				for (const auto& physicalDevice : physicalDevices)
				{
					auto properties = physicalDevice.getProperties();
					physicalDeviceDescriptions.Add(
						StringFormat("Device: %s\n", properties.deviceName)
						+ StringFormat(" Type: %s\n", vku::tools::PhysicalDeviceTypeString(properties.deviceType))
						+ StringFormat(" API: %i.%i.%i\n", properties.apiVersion >> 22, (properties.apiVersion >> 12) & 0x3ff, properties.apiVersion & 0xfff));
				}

				Verbose("Vulkan", "Available Vulkan devices\n" + StringJoin(physicalDeviceDescriptions, "\n"));
			}
		}
	}

	// Vulkan device creation
	// This is handled by a separate class that gets a logical device representation
	// and encapsulates functions related to a device
	vulkanDevice = new VulkanDevice(selectedPhysicalDevice);
	vk::PhysicalDeviceFeatures features;
	vk::Result res = vulkanDevice->CreateLogicalDevice(features, enabledDeviceExtensions, deviceCreatepNextChain);
	if (res != vk::Result::eSuccess)
	{
		PrintError("Vulkan", "Could not create Vulkan device: %s", vku::tools::ErrorString(res));
		return false;
	}
	device = vulkanDevice->logicalDevice;

	// Get a graphics queue from the device
	device.getQueue(vulkanDevice->queueFamilyIndices.graphics, 0, &queue);

	// Find a suitable depth format
	vk::Bool32 validDepthFormat = vku::tools::GetSupportedDepthFormat(selectedPhysicalDevice, &depthFormat);
	assert(validDepthFormat);

	swapChain.Connect(instance, selectedPhysicalDevice, device);

	// Create synchronization objects
	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	// Create a semaphore used to synchronize image presentation
	// Ensures that the image is displayed before we start submitting new commands to the queue
    CHECK_VULKAN_RESULT(device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphores.presentComplete));
	// Create a semaphore used to synchronize command submission
	// Ensures that the image is not presented until all commands have been submitted and executed
	CHECK_VULKAN_RESULT(device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphores.renderComplete));

	// Set up submit info structure
	// Semaphores will stay the same during application lifetime
	// Command buffer submission info is set by each example
	submitInfo = vk::SubmitInfo();
	submitInfo.pWaitDstStageMask = &submitPipelineStages;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &semaphores.presentComplete;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &semaphores.renderComplete;

	return true;
}

vk::Result Vulkan::CreateInstance()
{
	bool validation = DEBUG;

	vk::ApplicationInfo appInfo;
	appInfo.pApplicationName = Game::GetInstance()->GetInfo().title.c();
	appInfo.pEngineName = "Transparent Dragon";
	appInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

	// Enable surface extensions depending on os
#if PLATFORM_WINDOWS
	instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

	// Get extensions supported by the instance and store for later use
	uint32_t extCount = 0;
	CHECK_VULKAN_RESULT(vk::enumerateInstanceExtensionProperties(nullptr, &extCount, nullptr))
	if (extCount > 0)
	{
		std::vector<vk::ExtensionProperties> extensions(extCount);
		if (vk::enumerateInstanceExtensionProperties(nullptr, &extCount, &extensions.front()) == vk::Result::eSuccess)
		{
			for (vk::ExtensionProperties extension : extensions)
			{
				supportedInstanceExtensions.push_back(extension.extensionName);
			}
		}
	}

	// Enabled requested instance extensions
	if (enabledInstanceExtensions.size() > 0)
	{
		for (const char* enabledExtension : enabledInstanceExtensions)
		{
			// Output message if requested extension is not available
			if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), enabledExtension) == supportedInstanceExtensions.end())
			{
				PrintError("Vulkan", "Enabled instance extension %s is not present at instance level", enabledExtension);
			}
			instanceExtensions.push_back(enabledExtension);
		}
	}

	vk::InstanceCreateInfo instanceCreateInfo;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	if (instanceExtensions.size() > 0)
	{
		if (validation)
		{
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
	}
	if (validation)
	{
		// The VK_LAYER_KHRONOS_validation contains all current validation functionality.
		// Note that on Android this layer requires at least NDK r20
		const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
		// Check if this layer is available at instance level
		uint32_t instanceLayerCount;
		CHECK_VULKAN_RESULT(vk::enumerateInstanceLayerProperties(&instanceLayerCount, nullptr));
		std::vector<vk::LayerProperties> instanceLayerProperties(instanceLayerCount);
		CHECK_VULKAN_RESULT(vk::enumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data()));
		bool validationLayerPresent = false;
		for (vk::LayerProperties layer : instanceLayerProperties)
		{
			if (strcmp(layer.layerName, validationLayerName) == 0)
			{
				validationLayerPresent = true;
				break;
			}
		}
		if (validationLayerPresent)
		{
			instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
			instanceCreateInfo.enabledLayerCount = 1;
		}
		else
		{
			PrintError("Vulkan", "Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled");
		}
	}
	return vk::createInstance(&instanceCreateInfo, nullptr, &instance);
}

bool Vulkan::IsPhysicalDeviceSuitable(vk::PhysicalDevice device)
{
	vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
	vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

	auto extensions = device.enumerateDeviceExtensionProperties();
	for (auto requiredExtension : physicalDeviceExtensions)
	{
		bool extensionFound = false;
		for (auto extension : extensions)
		{
			if (strcmp(requiredExtension, extension.extensionName) == 0)
			{
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound)
		{
			return false;
		}
	}

	return deviceFeatures.geometryShader && deviceFeatures.samplerAnisotropy;
}

int Vulkan::RatePhysicalDeviceSuitability(vk::PhysicalDevice device)
{
	if (!IsPhysicalDeviceSuitable(device)) return 0;

	int score = 0;

	auto properties = device.getProperties();
	auto features = device.getFeatures();

	if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
	{
		score += 1000;
	}

	score += properties.limits.maxImageDimension2D;

	return score;
}

void Vulkan::InitSwapchain(class GameWindow* window)
{
#if PLATFORM_WINDOWS
	swapChain.InitSurface(window->hInstance, window->hWnd);
#endif
}

void Vulkan::CreateCommandPool()
{
	vk::CommandPoolCreateInfo cmdPoolInfo;
	cmdPoolInfo.queueFamilyIndex = swapChain.queueNodeIndex;
	cmdPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	CHECK_VULKAN_RESULT(device.createCommandPool(&cmdPoolInfo, nullptr, &cmdPool));
}

void Vulkan::SetupSwapChain(class GameWindow* window)
{
	swapChain.Create(&window->width, &window->height, true);
}

void Vulkan::CreateCommandBuffers()
{
	// Create one command buffer for each swap chain image and reuse for rendering
	drawCmdBuffers.resize(swapChain.imageCount);

	vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
	cmdBufAllocateInfo.commandPool = cmdPool;
	cmdBufAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
	cmdBufAllocateInfo.commandBufferCount = static_cast<uint32_t>(drawCmdBuffers.size());

	CHECK_VULKAN_RESULT(device.allocateCommandBuffers(&cmdBufAllocateInfo, drawCmdBuffers.data()));
}

void Vulkan::CreateSynchronizationPrimitives()
{
	// Wait fences to sync command buffer access
	vk::FenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
	waitFences.resize(drawCmdBuffers.size());
	for (auto& fence : waitFences)
	{
		CHECK_VULKAN_RESULT(device.createFence(&fenceCreateInfo, nullptr, &fence));
	}
}

void Vulkan::SetupDepthStencil(GameWindow* window)
{
	vk::ImageCreateInfo imageCI;
	imageCI.imageType = vk::ImageType::e2D;
	imageCI.format = depthFormat;
	imageCI.extent = vk::Extent3D{ window->width, window->height, 1 };
	imageCI.mipLevels = 1;
	imageCI.arrayLayers = 1;
	imageCI.samples = vk::SampleCountFlagBits::e1;
	imageCI.tiling = vk::ImageTiling::eOptimal;
	imageCI.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;

	CHECK_VULKAN_RESULT(device.createImage(&imageCI, nullptr, &depthStencil.image));
	vk::MemoryRequirements memReqs;
	device.getImageMemoryRequirements(depthStencil.image, &memReqs);

	vk::MemoryAllocateInfo memAllloc;
	memAllloc.allocationSize = memReqs.size;
	memAllloc.memoryTypeIndex = vulkanDevice->GetMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	CHECK_VULKAN_RESULT(device.allocateMemory(&memAllloc, nullptr, &depthStencil.mem));
	device.bindImageMemory(depthStencil.image, depthStencil.mem, 0);

	vk::ImageViewCreateInfo imageViewCI;
	imageViewCI.viewType = vk::ImageViewType::e2D;
	imageViewCI.image = depthStencil.image;
	imageViewCI.format = depthFormat;
	imageViewCI.subresourceRange.baseMipLevel = 0;
	imageViewCI.subresourceRange.levelCount = 1;
	imageViewCI.subresourceRange.baseArrayLayer = 0;
	imageViewCI.subresourceRange.layerCount = 1;
	imageViewCI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
	// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
	if (depthFormat >= vk::Format::eD16UnormS8Uint)
	{
		imageViewCI.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
	}
	CHECK_VULKAN_RESULT(device.createImageView(&imageViewCI, nullptr, &depthStencil.view));
}

void Vulkan::SetupRenderPass()
{
	std::array<vk::AttachmentDescription, 2> attachments;
	// Color attachment
	attachments[0].format = swapChain.colorFormat;
	attachments[0].samples = vk::SampleCountFlagBits::e1;
	attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
	attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
	attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	attachments[0].initialLayout = vk::ImageLayout::eUndefined;
	attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;
	// Depth attachment
	attachments[1].format = depthFormat;
	attachments[1].samples = vk::SampleCountFlagBits::e1;
	attachments[1].loadOp = vk::AttachmentLoadOp::eClear;
	attachments[1].storeOp = vk::AttachmentStoreOp::eStore;
	attachments[1].stencilLoadOp = vk::AttachmentLoadOp::eClear;
	attachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	attachments[1].initialLayout = vk::ImageLayout::eUndefined;
	attachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::AttachmentReference colorReference;
	colorReference.attachment = 0;
	colorReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::AttachmentReference depthReference;
	depthReference.attachment = 1;
	depthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::SubpassDescription subpassDescription;
	subpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorReference;
	subpassDescription.pDepthStencilAttachment = &depthReference;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;
	subpassDescription.pResolveAttachments = nullptr;

	// Subpass dependencies for layout transitions
	std::array<vk::SubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
	dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	dependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
	dependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();

	renderPass = device.createRenderPass(renderPassInfo);
}

void Vulkan::CreatePipelineCache()
{
	vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
	pipelineCache = device.createPipelineCache(pipelineCacheCreateInfo);
}

void Vulkan::SetupFrameBuffer(GameWindow* window)
{
	vk::ImageView attachments[2];

	// Depth/Stencil attachment is the same for all frame buffers
	attachments[1] = depthStencil.view;

	vk::FramebufferCreateInfo frameBufferCreateInfo;
	frameBufferCreateInfo.pNext = NULL;
	frameBufferCreateInfo.renderPass = renderPass;
	frameBufferCreateInfo.attachmentCount = 2;
	frameBufferCreateInfo.pAttachments = attachments;
	frameBufferCreateInfo.width = window->width;
	frameBufferCreateInfo.height = window->height;
	frameBufferCreateInfo.layers = 1;

	// Create frame buffers for every swap chain image
	frameBuffers.resize(swapChain.imageCount);
	for (uint32_t i = 0; i < frameBuffers.size(); i++)
	{
		attachments[0] = swapChain.buffers[i].view;
		CHECK_VULKAN_RESULT(device.createFramebuffer(&frameBufferCreateInfo, nullptr, &frameBuffers[i]));
	}
}

void Vulkan::DestroyCommandBuffers()
{
	device.freeCommandBuffers(cmdPool, static_cast<uint32_t>(drawCmdBuffers.size()), drawCmdBuffers.data());
}

void Vulkan::BuildCommandBuffers(GameWindow* window)
{
	vk::CommandBufferBeginInfo cmdBufInfo;

	VkClearValue clearValues[2];
	clearValues[0].color = { { 0.025f, 0.025f, 0.025f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = window->width;
	renderPassBeginInfo.renderArea.extent.height = window->height;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

	for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
	{
		renderPassBeginInfo.framebuffer = frameBuffers[i];

		CHECK_VULKAN_RESULT(drawCmdBuffers[i].begin(&cmdBufInfo));

		vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		drawCmdBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

		vk::Viewport viewport = vk::Viewport(0, 0, static_cast<float>(window->width), static_cast<float>(window->height), 0.0f, 1.0f);
		drawCmdBuffers[i].setViewport(0, 1, &viewport);

		vk::Rect2D scissor = vk::Rect2D(vk::Offset2D(0.0f, 0.0f), vk::Extent2D(window->width, window->height));
		drawCmdBuffers[i].setScissor(0, 1, &scissor);

		vk::DeviceSize offsets[1] = { 0 };
		model.bindBuffers(drawCmdBuffers[i]);

		/*
			[POI] Render cubes with separate descriptor sets
		*/
		for (auto cube : cubes) {
			// Bind the cube's descriptor set. This tells the command buffer to use the uniform buffer and image set for this cube
			vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &cube.descriptorSet, 0, nullptr);
			model.draw(drawCmdBuffers[i]);
		}

		drawUI(drawCmdBuffers[i]);

		vkCmdEndRenderPass(drawCmdBuffers[i]);

		CHECK_VULKAN_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
	}
}

VkBool32 Vulkan::VulkanValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	/*std::cmatch match;

	bool parseSuccess = false;
	if (std::regex_search(pCallbackData->pMessage, match, std::regex("\\[ ([\\w-]+) \\] ((?:Object \\d+: (?:handle = \\w+|VK_NULL_HANDLE), type = \\w+; )*)\\| MessageID = (\\w+) \\| (.*)\\. The Vulkan spec states: (.*) \\((.*)\\)")))
	{
		parseSuccess = true;
	}
	else if (std::regex_search(pCallbackData->pMessage, match, std::regex("\\[ ([\\w-]+) \\] ((?:Object \\d+: (?:handle = \\w+|VK_NULL_HANDLE), type = \\w+; )*)\\| MessageID = (\\w+) \\| (.*)")))
	{
		parseSuccess = true;
	}
	else
	{
		PrintError("Vulkan", pCallbackData->pMessage);
	}

	if (parseSuccess)
	{
		String info = match[4].str();
		for (int i = 1; String::ReplaceSingle(info, ": ", ":\n\t" + String("  ") * i); i++);

		String message = match[1].str() + " (" + match[3].str() + ")\n\t" + info;

		if (match[5].matched)
		{
			message += "\n\tUSG > " + match[5].str() + "\n\tSEE > " + match[6].str();
		}

		message += "\n\tObjects:";

		for (auto& object : String(match[2].str()).Split("; ", true))
		{
			std::cmatch objMatch;
			if (std::regex_search(object.c(), objMatch, std::regex("Object (\\d+): (?:handle = (\\w+)|(VK_NULL_HANDLE)), type = (\\w+)")))
			{
				message += "\n\t  " + objMatch[1].str() + ": " + objMatch[4].str() + " " + (objMatch[2].matched ? objMatch[2].str() : String::FormatPtr<void>(NULL));
			}
		}

		PrintError("Vulkan", message.c());
	}*/
	
	return VK_FALSE;
}
