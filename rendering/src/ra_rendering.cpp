#define VK_USE_PLATFORM_WIN32_KHR
// EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
#include <vector>
#include <set>
#include <string>
#include <algorithm>

// INTERNAL INCLUDES
#include "ra_rendering.h"
#include "ra_vkcheck.h"
#include "ra_types.h"
#include "ra_utils.h"
#include "ra_window.h"

void Rendering::Initialize(const char* engineName)
{
	this->deviceExtensions = 
	{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	this->validationLayers = 
	{
	"VK_LAYER_KHRONOS_validation"
	};

	CreateInstance(engineName);
	CreateSurface();
	PickPhysicalDevice();
	CreateDevice();
	CreateSwapChain();
	CreateImageViews();
}

void Rendering::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo = {	};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0;
	surfaceInfo.hinstance = GetModuleHandle(nullptr);
	surfaceInfo.hwnd = Window::GetInstancePtr()->GetHandle();

	this->surface = {};
	VK_CHECK(vkCreateWin32SurfaceKHR(this->instance, &surfaceInfo, nullptr, &this->surface));
}

void Rendering::PickPhysicalDevice()
{
	ui32 deviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr));

	std::vector<VkPhysicalDevice> devices(deviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &deviceCount, devices.data()));

	for (VkPhysicalDevice device : devices)
	{
		if (isPhysicalDeviceSuitable(device))
		{
			this->physicalDevice = device;
			break;
		}
	}
}

void Rendering::CreateInstance(const char* engineName)
{
	VkApplicationInfo appInfo = { };
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pEngineName = engineName;
	appInfo.pApplicationName = "Rendering Example";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	ui32 amountOfLayers = 0;
	VK_CHECK(vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr));

	VkLayerProperties* layers = new VkLayerProperties[amountOfLayers];
	VK_CHECK(vkEnumerateInstanceLayerProperties(&amountOfLayers, layers));

	ui32 amountOfExtensions = 0;
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr));

	VkExtensionProperties* extensions = new VkExtensionProperties[amountOfExtensions];
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensions));

	const std::vector<const char*> usedExtensions = {
		"VK_KHR_surface",
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	VkInstanceCreateInfo instInfo = { };
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext = nullptr;
	instInfo.pApplicationInfo = &appInfo;
	instInfo.flags = 0;
	instInfo.enabledLayerCount = static_cast<ui32>(this->validationLayers.size());
	instInfo.enabledExtensionCount = static_cast<ui32>(usedExtensions.size());
	instInfo.ppEnabledExtensionNames = usedExtensions.data();
	instInfo.ppEnabledLayerNames = this->validationLayers.data();

	this->instance = {};
	VK_CHECK(vkCreateInstance(&instInfo, nullptr, &this->instance));

	delete[] layers;
	delete[] extensions;
}

void Rendering::CreateDevice()
{
	QueueFamilyStats family = FindFamiliyQueues(this->physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { *family.graphicsFamily, *family.presentFamily};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueInfo = { };
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.pNext = nullptr;
		queueInfo.flags = 0;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &queuePriority;
		queueInfo.queueFamilyIndex = queueFamily;
		queueCreateInfos.push_back(queueInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo deviceInfo = { };
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceInfo.queueCreateInfoCount = static_cast<ui32>(queueCreateInfos.size());
	deviceInfo.enabledExtensionCount = static_cast<ui32>(this->deviceExtensions.size());
	deviceInfo.enabledLayerCount = static_cast<ui32>(this->validationLayers.size());
	deviceInfo.ppEnabledExtensionNames = this->deviceExtensions.data();
	deviceInfo.ppEnabledLayerNames = this->validationLayers.data();
	deviceInfo.pEnabledFeatures = &deviceFeatures;
	deviceInfo.flags = 0;

	this->logicalDevice = {};
	VK_CHECK(vkCreateDevice(this->physicalDevice, &deviceInfo, nullptr, &this->logicalDevice));

	VkQueue graphicsQueue = { };
	VkQueue presentQueue = {};
	vkGetDeviceQueue(this->logicalDevice, *family.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(this->logicalDevice, *family.presentFamily, 0, &presentQueue);

	delete family.graphicsFamily;
	delete family.presentFamily;
}

void Rendering::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = GetSwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = GetSwapChainSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = GetSwapChainPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = GetSwapChainExtent(swapChainSupport.capabilities);

	ui32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && 
		imageCount > swapChainSupport.capabilities.maxImageCount) 
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = this->surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VK_CHECK(vkCreateSwapchainKHR(this->logicalDevice, &createInfo, nullptr, &this->swapChain));

	VK_CHECK(vkGetSwapchainImagesKHR(this->logicalDevice, this->swapChain, &imageCount, nullptr));
	this->swapChainImages.resize(imageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(this->logicalDevice, this->swapChain, &imageCount, this->swapChainImages.data()));

	this->swapChainImageFormat = surfaceFormat.format;
	this->swapChainExtent = extent;

}

void Rendering::CreateImageViews()
{
	this->swapChainImageViews.resize(this->swapChainImages.size());

	for (ui32 i = 0; i < this->swapChainImages.size(); i++) 
	{
		this->swapChainImageViews[i] = CreateImageView(this->swapChainImages[i], this->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

VkImageView Rendering::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, ui32 mipLevels) 
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	VK_CHECK(vkCreateImageView(this->logicalDevice, &viewInfo, nullptr, &imageView));

	return imageView;
}

VkSurfaceFormatKHR Rendering::GetSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
	for (const VkSurfaceFormatKHR& availableFormat : availableFormats) 
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}

	return availableFormats[0];
}

VkPresentModeKHR Rendering::GetSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const VkPresentModeKHR& availablePresentMode : availablePresentModes) 
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
			return availablePresentMode;
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			bestMode = availablePresentMode;
	}

	return bestMode;
}

VkExtent2D Rendering::GetSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
{
	if (capabilities.currentExtent.width != _UI32_MAX)
		return capabilities.currentExtent;
	else 
	{
		VkExtent2D actualExtent =
		{
			Window::GetInstancePtr()->GetWidth(),
			Window::GetInstancePtr()->GetHeigth()
		};

		ui32 maxWidth = 0;
		ui32 minWidth = capabilities.minImageExtent.width;

		ui32 maxHeight = 0;
		ui32 minHeight = capabilities.minImageExtent.height;

		if (capabilities.maxImageExtent.width < actualExtent.width)
			maxWidth = capabilities.maxImageExtent.width;
		else
			maxWidth = actualExtent.width;

		if (capabilities.maxImageExtent.height < actualExtent.height)
			maxHeight = capabilities.maxImageExtent.height;
		else
			maxHeight = actualExtent.height;

		if (minHeight > maxHeight)
			actualExtent.height = minHeight;
		else
			actualExtent.height = maxHeight;

		if (minWidth > maxWidth)
			actualExtent.width = minWidth;
		else
			actualExtent.width = maxWidth;

		return actualExtent;
	}
}

bool Rendering::areExtensionSupported(VkPhysicalDevice device)
{
	ui32 extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

	std::set<std::string> requiredExtensions(this->deviceExtensions.begin(), this->deviceExtensions.end());

	for (const VkExtensionProperties& extension : extensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool Rendering::isPhysicalDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyStats family = FindFamiliyQueues(device);

	bool extensionsSupported = areExtensionSupported(device);

	bool swapChainSupported = false;

	if (extensionsSupported) 
	{
		SwapChainSupportDetails swapChainSupport = GetSwapChainSupport(device);
		swapChainSupported = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && family.isComplete() && swapChainSupported;
}

QueueFamilyStats Rendering::FindFamiliyQueues(VkPhysicalDevice device)
{
	QueueFamilyStats family;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (VkQueueFamilyProperties& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			if (family.graphicsFamily == nullptr)
				family.graphicsFamily = new ui32;
			*family.graphicsFamily = i;

		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport) 
		{
			if (family.presentFamily == nullptr)
				family.presentFamily = new ui32;
			*family.presentFamily = i;
		}

		if (family.isComplete())
		{
			break;
		}

		i++;
	}

	return family;
}

SwapChainSupportDetails Rendering::GetSwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->surface, &details.capabilities);

	ui32 formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surface, &formatCount, nullptr);

	if (formatCount != 0) 
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	ui32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

void Rendering::Cleanup()
{
	vkDestroyDevice(this->logicalDevice, nullptr);
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyInstance(this->instance, nullptr);
} 
