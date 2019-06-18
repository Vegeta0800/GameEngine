
#define VK_USE_PLATFORM_WIN32_KHR
// EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
#include <iostream>

// INTERNAL INCLUDES
#include "ra_rendering.h"
#include "ra_vkcheck.h"
#include "ra_types.h"
#include "ra_utils.h"
#include "ra_window.h"

void Rendering::Initialize(const char* applicationName, ui32 applicationVersion)
{
	this->CreateInstance(applicationName, applicationVersion);
	this->CreatePhysicalDevice();
	this->CreateSurface();
	this->CreateLogicalDevice();
}

void Rendering::CreateInstance(const char* applicationName, ui32 applicationVersion)
{
	const std::vector<const char*> instanceValidationLayers =
	{
		"VK_LAYER_LUNARG_standard_validation"
	};

	const std::vector<const char*> instanceExtensions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = applicationName;
	appInfo.applicationVersion = applicationVersion;
	appInfo.pEngineName = "RA Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	//ui32 supportedLayerCount = 0;
	//VK_CHECK(vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr));

	//VkLayerProperties* layers = new VkLayerProperties[supportedLayerCount];
	//VK_CHECK(vkEnumerateInstanceLayerProperties(&supportedLayerCount, layers));

	//ui32 extensionCount = 0;
	//VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));

	//VkExtensionProperties* extensions = new VkExtensionProperties[extensionCount];
	//VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions));

	//for (ui32 i = 0; i < extensionCount; i++)
	//	printf("Name:    %s \n", extensions[i].extensionName);

	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0; //TODO
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = static_cast<ui32>(instanceValidationLayers.size());
	instanceInfo.ppEnabledLayerNames = instanceValidationLayers.data();
	instanceInfo.enabledExtensionCount = static_cast<ui32>(instanceExtensions.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data(); 

	VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &this->instance));

	//delete[] layers;
	//delete[] extensions;
}

void Rendering::CreatePhysicalDevice()
{
	ui32 physicalDeviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr));

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, physicalDevices.data()));

	PickIdealPhysicalDevice(physicalDevices);
}

void Rendering::PickIdealPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices)
{
	std::vector<ui32> ratings;

	for (const VkPhysicalDevice& phyDevice : physicalDevices)
	{
		ui32 rating = 0;

		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(phyDevice, &properties);

		switch (properties.deviceType)
		{
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				rating += 1000;
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				rating += 500;
				break;
			default:
				rating += 10;
				break;
		}

		VkPhysicalDeviceFeatures deviceFeatures = { };
		vkGetPhysicalDeviceFeatures(phyDevice, &deviceFeatures);

		if (deviceFeatures.geometryShader)
			rating += 300;

		ui32 queueFamiliyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamiliyCount, nullptr);

		std::vector<VkQueueFamilyProperties> familiyProperties(queueFamiliyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamiliyCount, familiyProperties.data());

		for (int i = 0; i < familiyProperties.size(); i++)
		{
			if ((familiyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				rating += 100;
			if ((familiyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
				rating += 5;
			if ((familiyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
				rating += 10;
			if ((familiyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)
				rating += 10;
			if ((familiyProperties[i].queueFlags & VK_QUEUE_PROTECTED_BIT) != 0)
				rating += 10;

			rating += familiyProperties[i].queueCount;
		}

		ratings.push_back(rating);
	}

	ui32 highestValue = 0;

	for (int i = 0; i < ratings.size(); i++)
	{
		if (ratings[i] > highestValue)
		{
			this->physicalDevice = physicalDevices[i];
			highestValue = ratings[i];
		}
	}

}

void Rendering::CreateLogicalDevice()
{
	float queuePriority = 1.0f;

	VkDeviceQueueCreateInfo queueInfo;
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = nullptr;
	queueInfo.flags = 0;
	queueInfo.queueFamilyIndex = 0;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures enabledFeatures = { };

	VkDeviceCreateInfo deviceInfo;
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.flags = 0;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledLayerNames = nullptr;
	deviceInfo.enabledExtensionCount = 0;
	deviceInfo.ppEnabledExtensionNames = nullptr;
	deviceInfo.pEnabledFeatures = &enabledFeatures;

	VK_CHECK(vkCreateDevice(this->physicalDevice, &deviceInfo, nullptr, &this->logicalDevice));

	VkQueue queue = { };
	vkGetDeviceQueue(this->logicalDevice, 0, 0, &queue);

}

void Rendering::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo;
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0; //TODO
	surfaceInfo.hinstance = GetModuleHandle(NULL);
	surfaceInfo.hwnd = Window::GetInstancePtr()->GetHandle();

	VK_CHECK(vkCreateWin32SurfaceKHR(this->instance, &surfaceInfo, nullptr, &this->surface));

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDevice, this->surface, &surfaceCapabilities));
	
	//std::cout << "Surface Capabilities: " << std::endl;
	//std::cout << "\tminImageCount: " << surfaceCapabilities.minImageCount << std::endl;
	//std::cout << "\tmaxImageCount: " << surfaceCapabilities.maxImageCount << std::endl;
	//std::cout << "\tcurrentExtent: " << surfaceCapabilities.currentExtent.width << "/" << surfaceCapabilities.currentExtent.height << std::endl;
	//std::cout << "\tminImageExtent: " << surfaceCapabilities.minImageExtent.width << "/" << surfaceCapabilities.minImageExtent.height << std::endl;
	//std::cout << "\tmaxImageExtent: " << surfaceCapabilities.maxImageExtent.width << "/" << surfaceCapabilities.maxImageExtent.height << std::endl;
	//std::cout << "\tmaxImageArrayLayers: " << surfaceCapabilities.maxImageArrayLayers << std::endl;
	//std::cout << "\tsupportedTransforms: " << surfaceCapabilities.supportedTransforms << std::endl;
	//std::cout << "\tcurrentTransform: " << surfaceCapabilities.currentTransform << std::endl;
	//std::cout << "\tsupportedCompositeAlpha: " << surfaceCapabilities.supportedCompositeAlpha << std::endl;
	//std::cout << "\tsupportedUsageFlags: " << surfaceCapabilities.supportedUsageFlags << std::endl;

	ui32 formatsCount = 0;
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, this->surface, &formatsCount, nullptr));

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, this->surface, &formatsCount, surfaceFormats.data()));


}

void Rendering::Cleanup()
{
	VK_CHECK(vkDeviceWaitIdle(this->logicalDevice));

	vkDestroyDevice(this->logicalDevice, nullptr);
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyInstance(this->instance, nullptr);
} 
