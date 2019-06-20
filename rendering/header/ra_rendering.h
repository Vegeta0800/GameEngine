
#pragma once
//EXTERNAL INCLUDES
#include "vulkan/vulkan.h"
#include <vector>
//INTERNAL INCLUDES
#include "ra_types.h"


class Rendering 
{
	enum class RenderingBuffer
	{
		VERTEX = 0,
		FRAGMENT = 1,
		GEOMETRY = 2
	};

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

	void CreateSwapChain(void);
	void CreateImageViews(void);
	bool isModeSupported(const std::vector<VkPresentModeKHR>& supportedPresentModes, VkPresentModeKHR presentMode);

	void CreateShaderModules(void);
	void CreateShaderModule(const std::vector<byte>& code, VkShaderModule* shaderModule);

	std::vector<byte> GetBuffer(RenderingBuffer bufferType);

	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSurfaceKHR surface;

	VkSwapchainKHR swapchain;
	std::vector<VkImageView> imageViews;

	VkShaderModule vertexModule;
	VkShaderModule fragmentModule;
};