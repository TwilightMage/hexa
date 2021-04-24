#pragma once

#include "framework.h"

#include <vulkan/vulkan.hpp>

#include "Game.h"
#include "String.h"

#define DEFAULT_FENCE_TIMEOUT 100000000000

#define CHECK_VULKAN_RESULT(f)																						                        \
{																													                        \
    vk::Result res = (f);																							                        \
    if (res != vk::Result::eSuccess)																				                        \
    {																												                        \
        PrintError("Vulkan", "VkResult is %s in %s at line %i", vku::tools::ErrorString(res), __FILENAME__, __LINE__);	\
    assert(false);																							    	                        \
    }																												                        \
}

namespace vku
{
    namespace tools
    {
        inline bool errorModeSilent = false;

		const char* ErrorString(vk::Result errorCode);
		String PhysicalDeviceTypeString(vk::PhysicalDeviceType type);

        vk::Bool32 GetSupportedDepthFormat(vk::PhysicalDevice physicalDevice, vk::Format* depthFormat);

        void ExitFatal(const String& message, int32_t exitCode);
        void ExitFatal(const String& message, vk::Result resultCode);
    }
}
