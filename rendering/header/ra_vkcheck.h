#pragma once

// EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
#include <cstdio>
// INTERNAL INCLUDES

inline const char* GetVulkanResultString(const VkResult& result)
{
	const char* str = "Unknown";
	switch (result)
	{
	case VK_NOT_READY:
		str = "A fence or query has not yet completed.";
		break;
	case VK_TIMEOUT:
		str = "A wait operation has not completed in the specified time.";
		break;
	case VK_EVENT_SET:
		str = "An event is signaled.";
		break;
	case VK_EVENT_RESET:
		str = "An event is unsignaled.";
		break;
	case VK_INCOMPLETE:
		str = "A return array was too small for the result.";
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		str = "A host memory allocation has failed.";
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		str = "A device memory allocation has failed.";
		break;
	case VK_ERROR_INITIALIZATION_FAILED:
		str = "Initialization of an object could not be completed for implementation-specific reasons.";
		break;
	case VK_ERROR_DEVICE_LOST:
		str = "The logical or physical device has been lost.";
		break;
	case VK_ERROR_MEMORY_MAP_FAILED:
		str = "Mapping of a memory object has failed.";
		break;
	case VK_ERROR_LAYER_NOT_PRESENT:
		str = "A requested layer is not present or could not be loaded.";
		break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		str = "A requested extension is not supported.";
		break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
		str = "A requested feature is not supported.";
		break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		str = "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.";
		break;
	case VK_ERROR_TOO_MANY_OBJECTS:
		str = "Too many objects of the type have already been created.";
		break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		str = "A requested format is not supported on this device.";
		break;
	case VK_ERROR_FRAGMENTED_POOL:
		str = "A pool allocation has failed due to fragmentation of the pool's memory.";
		break;
	case VK_ERROR_OUT_OF_POOL_MEMORY:
		str = "A pool memory allocation has failed.";
		break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
		str = "An external handle is not a valid handle of the specified type.";
		break;
	case VK_ERROR_SURFACE_LOST_KHR:
		str = "A surface is no longer available.";
		break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		str = "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.";
		break;
	case VK_SUBOPTIMAL_KHR:
		str = "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.";
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		str = "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail.";
		break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		str = "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.";
		break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
		str = "An extension could not be validated.";
		break;
	case VK_ERROR_INVALID_SHADER_NV:
		str = "One or more shaders failed to compile or link.";
		break;
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
		break;
	case VK_ERROR_FRAGMENTATION_EXT:
		str = "A descriptor pool creation has failed due to fragmentation.";
		break;
	case VK_ERROR_NOT_PERMITTED_EXT:
		break;
	case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
		str = "A buffer creation failed because the requested address is not available.";
		break;
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
		str = "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access.";
		break;
	case VK_SUCCESS:
	default:
		str = "Command successfully completed.";
		break;
	}
	return str;
}

#if defined (RA_COMPILE_RELEASE)
#define VK_CHECK(x)															\
{																			\
	auto hr = x;															\
    if (hr != VK_SUCCESS)                                                   \
    {                                                                       \
        printf("Vulkan error: [%s]\n", GetVulkanResultString(hr));          \
        return;                                                             \
    }																		\
}
#else
#define VK_CHECK(x)															\
{																			\
	auto hr = x;															\
    if (hr != VK_SUCCESS)                                                   \
    {                                                                       \
        printf("Vulkan error: [%s]\n", GetVulkanResultString(hr));          \
        __debugbreak();														\
    }                                                                       \
    else                                                                    \
    {                                                                       \
        printf("Vulkan action: %s [%s]\n", #x, GetVulkanResultString(hr));	\
    }																		\
}
#endif
