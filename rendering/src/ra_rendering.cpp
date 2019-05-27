// EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
// INTERNAL INCLUDES
#include "ra_rendering.h"
#include "ra_vkcheck.h"
#include "ra_types.h"

void Rendering::Initialize(const char* engineName)
{
	VkApplicationInfo appInfo = { };
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pEngineName = engineName;
	appInfo.pApplicationName = "Rendering Example";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo instInfo = { };
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext = nullptr;
	instInfo.pApplicationInfo = &appInfo;
	instInfo.flags = 0;
	instInfo.enabledLayerCount = 0;
	instInfo.enabledExtensionCount = 0;
	instInfo.ppEnabledExtensionNames = nullptr;
	instInfo.ppEnabledLayerNames = nullptr;

	VkInstance instance = { };
	VK_CHECK(vkCreateInstance(&instInfo, nullptr, &instance));

	ui32 deviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));

	VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices));

	VkPhysicalDeviceProperties2 devProps = { };
	devProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	vkGetPhysicalDeviceProperties2(devices[0], &devProps);

	VkPhysicalDeviceMemoryProperties2 memProps = { };
	devProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
	vkGetPhysicalDeviceMemoryProperties2(devices[0], &memProps);

	printf("Device found: %s @ %.3f GB\n"
		, devProps.properties.deviceName
		, memProps.memoryProperties.memoryHeaps[0].size / 10e8);

	VkDeviceQueueCreateInfo queueInfo = { };
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = nullptr;
	queueInfo.flags = 0;
	queueInfo.queueCount = 1; //TODO
	queueInfo.pQueuePriorities = 0; //TODO
	queueInfo.queueFamilyIndex = 0; //TODO

	VkPhysicalDeviceFeatures deviceFeatures = { };

	VkDeviceCreateInfo deviceInfo = { };
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.enabledExtensionCount = 0;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledExtensionNames = nullptr;
	deviceInfo.ppEnabledLayerNames = nullptr;
	deviceInfo.pEnabledFeatures = &deviceFeatures; //TODO
	deviceInfo.flags = 0;

	VkDevice device = { };
	VK_CHECK(vkCreateDevice(devices[0], &deviceInfo, nullptr, &device));

	delete[] devices;
}
