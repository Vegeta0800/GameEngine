
#pragma once
//EXTERNAL INCLUDES
#include "vulkan/vulkan.h"
#include <vector>
#include <chrono>

//INTERNAL INCLUDES
#include "ra_vertex.h"
#include "ra_types.h"
#include "math/ra_vector3.h"
#include "math/ra_mat4x4.h"

class Gameobject;

class Rendering 
{
	enum class RenderingBuffer
	{
		VERTEX = 0,
		FRAGMENT = 1
	};

public:
	void Initialize(const char* applicationName, ui32 applicationVersion);
	void Update(float time);
	void Cleanup(void);

	void RecreateSwapchain(void);

	bool GetInitStatus(void);
private:
	void DrawFrame(void);
	void UpdateMVP(float time); //TODO

	void CreateInstance(const char* applicationName, ui32 applicationVersion);

	void CreatePhysicalDevice(void);
	void PickIdealPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices);

	void SetupQueues(void);

	void CreateLogicalDevice(void);

	void CreateSurface(void);

	void CreateSwapChain(void);
	void CreateImageViews(void);
	bool isModeSupported(const std::vector<VkPresentModeKHR>& supportedPresentModes, VkPresentModeKHR presentMode);

	void CreateShaderModules(void);
	void CreateShaderModule(const std::vector<byte>& code, VkShaderModule* shaderModule);

	void CreateDescriptorSetLayout(void);
	void CreateDescriptorPool(void);
	void CreateDescriptorSet(void);

	void CreatePipeline(void);
	void CreateRenderpass(void);

	void CreateFramebuffers(void);

	void CreateCommandbuffer(void);

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsageFlags, VkBuffer& buffer, VkMemoryPropertyFlags memoryFlags, VkDeviceMemory& memory);
	void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

	template <typename T>
	void CreateBufferOnGPU(std::vector<T> data, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory);

	void CreateUniformbuffer(void);

	void CreateVertexbuffers();
	void CreateIndexbuffers();

	void RecordCommands(void);

	void CreateSemaphores(void);

	std::vector<byte> GetBuffer(RenderingBuffer bufferType);

	ui32 FindMemoryTypeIndex(ui32 typeFilter, VkMemoryPropertyFlags properties);

	int indexOfGraphicsQueue = INT_MAX;
	int indexOfPresentQueue = INT_MAX;
	int indexOfTransferQueue = INT_MAX;
	bool initialized = false;

	bool canGraphicsQueuePresent = false;

	std::vector<Gameobject*> gameObjects;
	
	ui16 queueCount = 0;

	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSurfaceKHR surface;

	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkQueue transferQueue = VK_NULL_HANDLE;
	std::vector<ui32> queueFamilyIndices;

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

	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkBuffer> indexBuffers;
	std::vector<VkBuffer> uniformBuffers;


	std::vector<VkDeviceMemory> indexBufferMemories;
	std::vector<VkDeviceMemory> vertexBufferMemories;
	std::vector<VkDeviceMemory> uniformBufferMemories;

	VkBuffer uniformBuffer;

	VkDeviceMemory uniformBufferMemory;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	Gameobject* root;
	Gameobject* testObject;
	Gameobject* testObject2;

	Math::Mat4x4 mvp;
	Math::Vec3 cameraPos;
};