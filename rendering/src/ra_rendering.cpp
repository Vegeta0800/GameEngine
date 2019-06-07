#define VK_USE_PLATFORM_WIN32_KHR
// EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
#include <vector>
#include <set>
// INTERNAL INCLUDES
#include "ra_rendering.h"
#include "ra_vkcheck.h"
#include "ra_types.h"

void Rendering::Initialize(const char* engineName, HWND windowHandle)
{
	CreateInstance(engineName);
	CreateSurface(windowHandle);
	PickPhysicalDevice();
	CreateDevice();
}

void Rendering::CreateSurface(HWND windowHandle)
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo = {	};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0;
	surfaceInfo.hinstance = GetModuleHandle(nullptr);
	surfaceInfo.hwnd = windowHandle;

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
		queueInfo.queueFamilyIndex = *family.graphicsFamily;
		queueCreateInfos.push_back(queueInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo deviceInfo = { };
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceInfo.queueCreateInfoCount = static_cast<ui32>(queueCreateInfos.size());
	deviceInfo.enabledExtensionCount = 0;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledExtensionNames = nullptr;
	deviceInfo.ppEnabledLayerNames = nullptr;
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

bool Rendering::isPhysicalDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyStats family = FindFamiliyQueues(device);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && family.isComplete();
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

void Rendering::Cleanup()
{
	VK_CHECK(vkDeviceWaitIdle(this->logicalDevice));

	vkDestroyDevice(this->logicalDevice, nullptr);
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyInstance(this->instance, nullptr);
}
