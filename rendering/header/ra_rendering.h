
#pragma once
//EXTERNAL INCLUDES
#include "vulkan/vulkan.h"
//INTERNAL INCLUDES

class Rendering 
{
public:

	void Initialize(const char* engineName, HWND windowHandle);
	void Cleanup();

private:
	VkInstance instance;
	VkDevice device;
	VkSurfaceKHR surface;
};