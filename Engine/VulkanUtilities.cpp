#include "VulkanUtilities.h"

const char* vku::tools::ErrorString(vk::Result errorCode)
{
	const VkResult _errorCode = static_cast<VkResult>(errorCode);
	switch (_errorCode)
	{
#define STR(r) case VK_ ##r: return #r
		STR(NOT_READY);;
		STR(TIMEOUT);
		STR(EVENT_SET);
		STR(EVENT_RESET);
		STR(INCOMPLETE);
		STR(ERROR_OUT_OF_HOST_MEMORY);
		STR(ERROR_OUT_OF_DEVICE_MEMORY);
		STR(ERROR_INITIALIZATION_FAILED);
		STR(ERROR_DEVICE_LOST);
		STR(ERROR_MEMORY_MAP_FAILED);
		STR(ERROR_LAYER_NOT_PRESENT);
		STR(ERROR_EXTENSION_NOT_PRESENT);
		STR(ERROR_FEATURE_NOT_PRESENT);
		STR(ERROR_INCOMPATIBLE_DRIVER);
		STR(ERROR_TOO_MANY_OBJECTS);
		STR(ERROR_FORMAT_NOT_SUPPORTED);
		STR(ERROR_SURFACE_LOST_KHR);
		STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		STR(SUBOPTIMAL_KHR);
		STR(ERROR_OUT_OF_DATE_KHR);
		STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		STR(ERROR_VALIDATION_FAILED_EXT);
		STR(ERROR_INVALID_SHADER_NV);
#undef STR
	default:
		return "UNKNOWN_ERROR";
	}
}

String vku::tools::PhysicalDeviceTypeString(vk::PhysicalDeviceType type)
{
	const VkPhysicalDeviceType _type = static_cast<VkPhysicalDeviceType>(type);
	switch (_type)
	{
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_ ##r: return #r
		STR(OTHER);
		STR(INTEGRATED_GPU);
		STR(DISCRETE_GPU);
		STR(VIRTUAL_GPU);
#undef STR
	default: return "UNKNOWN_DEVICE_TYPE";
	}
}

vk::Bool32 vku::tools::GetSupportedDepthFormat(vk::PhysicalDevice physicalDevice, vk::Format* depthFormat)
{
	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	std::vector<vk::Format> depthFormats = {
		vk::Format::eD32SfloatS8Uint,
		vk::Format::eD32Sfloat,
		vk::Format::eD24UnormS8Uint,
		vk::Format::eD16UnormS8Uint,
		vk::Format::eD16Unorm
	};

	for (auto& format : depthFormats)
	{
		vk::FormatProperties formatProps;
		physicalDevice.getFormatProperties(format, &formatProps);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProps.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
		{
			*depthFormat = format;
			return true;
		}
	}

	return false;
}

void vku::tools::ExitFatal(const String& message, int32_t exitCode)
{
#if PLATFORM_WINDOWS
	if (!errorModeSilent)
	{
		MessageBox(NULL, message.wc(), NULL, MB_OK | MB_ICONERROR);
	}
#endif
	PrintError("Vulkan", message);
	exit(exitCode);
}

void vku::tools::ExitFatal(const String& message, vk::Result resultCode)
{
	ExitFatal(message, static_cast<int32_t>(resultCode));
}
