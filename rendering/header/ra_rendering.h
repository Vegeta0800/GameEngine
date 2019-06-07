
#pragma once
//EXTERNAL INCLUDES
#include "vulkan/vulkan.h"
#include <optional>
//INTERNAL INCLUDES
#include "ra_types.h"

struct QueueFamilyStats
{
	ui32* graphicsFamily = nullptr;
	ui32* presentFamily = nullptr;

	bool isComplete()
	{
		return (graphicsFamily != nullptr) && (presentFamily != nullptr);
	}
};

class Rendering 
{
public:

	void Initialize(const char* engineName, HWND windowHandle);
	void Cleanup(void);

private:
	void CreateSurface(HWND windowHandle);
	void PickPhysicalDevice();
	void CreateInstance(const char* engineName);
	void CreateDevice();
	bool isPhysicalDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyStats FindFamiliyQueues(VkPhysicalDevice device);

	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSurfaceKHR surface;
};