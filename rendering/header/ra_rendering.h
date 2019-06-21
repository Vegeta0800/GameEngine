
#pragma once
//EXTERNAL INCLUDES
#include "vulkan/vulkan.h"
#include <vector>
#include <chrono>

//INTERNAL INCLUDES
#include "ra_types.h"
#include "math/ra_vector3.h"

class Gameobject;

class Rendering 
{
	enum class RenderingBuffer
	{
		VERTEX = 0,
		FRAGMENT = 1
	};

	struct Vertex
	{
		Math::Vec3 position;
		Math::Vec3 color;

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription vertexInputBindingDescription;
			vertexInputBindingDescription.binding = 0;
			vertexInputBindingDescription.stride = sizeof(Vertex);
			vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return vertexInputBindingDescription;

		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription(2);

			vertexInputAttributeDescription[0].location = 0;
			vertexInputAttributeDescription[0].binding = 0;
			vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			vertexInputAttributeDescription[0].offset = offsetof(Vertex, position);

			vertexInputAttributeDescription[1].location = 1;
			vertexInputAttributeDescription[1].binding = 0;
			vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			vertexInputAttributeDescription[1].offset = offsetof(Vertex, color);

			return vertexInputAttributeDescription;
		}
	};

public:
	void Initialize(const char* applicationName, ui32 applicationVersion);
	void Update(void);
	void Cleanup(void);

	void RecreateSwapchain(void);

	bool GetInitStatus(void);
private:
	void DrawFrame(void);
	void UpdateMVP(void); //TODO

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

	void CreateVertexbuffer(void);
	void CreateIndexbuffer(void);
	void CreateUniformbuffer(void);


	void RecordCommands(void);

	void CreateSemaphores(void);

	std::vector<byte> GetBuffer(RenderingBuffer bufferType);

	ui32 FindMemoryTypeIndex(ui32 typeFilter, VkMemoryPropertyFlags properties);

	int indexOfGraphicsQueue = INT_MAX;
	int indexOfPresentQueue = INT_MAX;
	int indexOfTransferQueue = INT_MAX;
	bool initialized = false;

	bool canGraphicsQueuePresent = false;

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

	std::vector<Vertex> vertices;
	std::vector<ui32> indicies;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderingFinishedSemaphore;

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkBuffer uniformBuffer;

	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory uniformBufferMemory;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	Gameobject* root;
	Gameobject* testObject;

};