
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
		FRAGMENT = 1
	};

public:
	void Initialize(const char* applicationName, ui32 applicationVersion);
	void Update(void);
	void Cleanup(void);

private:
	void DrawFrame(void);

	void CreateInstance(const char* applicationName, ui32 applicationVersion);

	void CreatePhysicalDevice(void);
	void PickIdealPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices);

	void CreateLogicalDevice(void);

	void CreateSurface(void);

	void CreateSwapChain(void);
	void RecreateSwapChain(void);
	void CreateImageViews(void);
	bool isModeSupported(const std::vector<VkPresentModeKHR>& supportedPresentModes, VkPresentModeKHR presentMode);

	void CreateShaderModules(void);
	void CreateShaderModule(const std::vector<byte>& code, VkShaderModule* shaderModule);

	void CreatePipeline(void);

	void CreateFramebuffers(void);

	void CreateCommandbuffer(void);

	void RecordCommands(void);

	void CreateSemaphores(void);

	std::vector<byte> GetBuffer(RenderingBuffer bufferType);

	ui16 indexOfGraphicsQueue;

	VkQueue queue;

	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSurfaceKHR surface;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	std::vector<VkImageView> imageViews;
	std::vector<VkImage> swapchainImages;

	VkShaderModule vertexModule;
	VkShaderModule fragmentModule;

	VkRenderPass renderpass;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkFramebuffer> framebuffers;
	std::vector<VkPipelineStageFlags> waitStageMask;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderingFinishedSemaphore;
};