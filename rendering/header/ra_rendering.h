
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

struct SwapChainSupportDetails 
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Rendering 
{
public:
	void Initialize(const char* engineName);
	void Update(void);
	void Cleanup(void);

private:
	void CreateSurface(void);
	void PickPhysicalDevice(void);
	void CreateInstance(const char* engineName);
	void CreateDevice(void);
	void CreateSwapChain(void);
	void CreateImageViews(void);

	bool isPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool areExtensionSupported(VkPhysicalDevice device);

	QueueFamilyStats FindFamiliyQueues(VkPhysicalDevice device);
	SwapChainSupportDetails GetSwapChainSupport(VkPhysicalDevice device);

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, ui32 mipLevels);
	VkSurfaceFormatKHR GetSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR GetSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D GetSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSurfaceKHR surface;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<const char*> deviceExtensions;
	std::vector<const char*> validationLayers;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
};