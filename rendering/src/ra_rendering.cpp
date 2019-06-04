
#define VK_USE_PLATFORM_WIN32_KHR
// EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
#include <vector>
// INTERNAL INCLUDES
#include "ra_rendering.h"
#include "ra_vkcheck.h"
#include "ra_types.h"

void Rendering::Initialize(const char* engineName, HWND windowHandle)
{
	this->instance = {};

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

	std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	std::vector<const char*> usedExtensions = {
		"VK_KHR_surface",
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	VkInstanceCreateInfo instInfo = { };
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext = nullptr;
	instInfo.pApplicationInfo = &appInfo;
	instInfo.flags = 0;
	instInfo.enabledLayerCount = static_cast<ui32>(validationLayers.size());
	instInfo.enabledExtensionCount = static_cast<ui32>(usedExtensions.size());
	instInfo.ppEnabledExtensionNames = usedExtensions.data();
	instInfo.ppEnabledLayerNames = validationLayers.data();

	this->instance = {};
	VK_CHECK(vkCreateInstance(&instInfo, nullptr, &this->instance));

	VkWin32SurfaceCreateInfoKHR surfaceInfo = {	};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0;
	surfaceInfo.hinstance = nullptr;
	surfaceInfo.hwnd = nullptr;
	
	this->surface = {};
	VK_CHECK(vkCreateWin32SurfaceKHR(this->instance, &surfaceInfo, nullptr, &this->surface));

	ui32 deviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr));

	VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
	VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &deviceCount, devices));

	VkPhysicalDeviceProperties2 devProps = { };
	devProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	vkGetPhysicalDeviceProperties2(devices[0], &devProps);

	VkPhysicalDeviceMemoryProperties2 memProps = { };
	memProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
	vkGetPhysicalDeviceMemoryProperties2(devices[0], &memProps);

	printf("Device found: %s @ %.3f GB\n"
		, devProps.properties.deviceName
		, memProps.memoryProperties.memoryHeaps[0].size / 10e8);

	float queuePriorities[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	ui32 amountOfQueueFamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &amountOfQueueFamilies, nullptr);

	VkQueueFamilyProperties* queueFamiliyProps = new VkQueueFamilyProperties[amountOfQueueFamilies];
	vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &amountOfQueueFamilies, queueFamiliyProps);

	VkDeviceQueueCreateInfo queueInfo = { };
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = nullptr;
	queueInfo.flags = 0;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = queuePriorities;
	queueInfo.queueFamilyIndex = 0; //TODO

	VkPhysicalDeviceFeatures features = { };
	vkGetPhysicalDeviceFeatures(devices[0], &features);

	VkDeviceCreateInfo deviceInfo = { };
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.enabledExtensionCount = 0;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledExtensionNames = nullptr;
	deviceInfo.ppEnabledLayerNames = nullptr;
	deviceInfo.pEnabledFeatures = &features;
	deviceInfo.flags = 0;

	this->device = {};
	VK_CHECK(vkCreateDevice(devices[0], &deviceInfo, nullptr, &this->device));

	VkQueue queue = { };
	vkGetDeviceQueue(this->device, 0, 0, &queue);

	delete[] layers;
	delete[] extensions;
	delete[] queueFamiliyProps;
	delete[] devices;
}

void Rendering::Cleanup()
{
	VK_CHECK(vkDeviceWaitIdle(this->device));

	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyDevice(this->device, nullptr);
	vkDestroyInstance(this->instance, nullptr);
}
