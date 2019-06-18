
#pragma once
//EXTERNAL INCLUDES
#include "vulkan/vulkan.h"
#include <vector>
//INTERNAL INCLUDES
#include "ra_types.h"

class Rendering 
{
public:
	void Initialize(const char* applicationName, ui32 applicationVersion);
	void Update(void);
	void Cleanup(void);

private:
	void CreateInstance(const char* applicationName, ui32 applicationVersion);

	void CreatePhysicalDevice(void);
	void PickIdealPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices);

	void CreateLogicalDevice(void);

	void CreateSurface(void);

	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSurfaceKHR surface;
};