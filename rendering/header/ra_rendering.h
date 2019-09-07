
#pragma once
//EXTERNAL INCLUDES
#include <vector>
#include <chrono>

//INTERNAL INCLUDES
#include "ra_vertex.h"
#include "ra_types.h"
#include "ra_utils.h"
#include "math/ra_vector3.h"
#include "math/ra_mat4x4.h"
#include "ra_texture.h"
#include "ra_depthimage.h"
#include "ra_model.h"

#define INSTANCE_AMOUNT 1000

class Gameobject;

class Rendering 
{
	DEFINE_SINGLETON(Rendering)

	enum class RenderingBuffer
	{
		VERTEX = 0,
		INSTANCED = 1,
		FRAGMENT = 2,
	};

	struct alignas(16) VertexInputInfo
	{
		Math::Mat4x4 modelMatrix;
		Math::Mat4x4 viewMatrix;
		Math::Mat4x4 projectionMatrix;
		fColorRGBA color;
		Math::Vec3 lightPosition;
		float ambientVal;
		float specularVal;
	};

	struct alignas(16) VertexInstancedInputInfo
	{
		Math::Mat4x4 modelMatrix[INSTANCE_AMOUNT];
		Math::Mat4x4 viewMatrix;
		Math::Mat4x4 projectionMatrix;
		fColorRGBA color;
		Math::Vec3 lightPosition;
		float ambientVal;
		float specularVal;
	};

public:
	void Initialize(const char* applicationName, ui32 applicationVersion);
	void Update();
	void Cleanup(void);

	void RecreateSwapchain(void);
	
	ui32 FindMemoryTypeIndex(ui32 typeFilter, VkMemoryPropertyFlags properties);

	std::vector<Vertex> GetVerticesOfObject(std::string name);
	std::vector<ui32> GetIndicesOfObject(std::string name);

	VkDevice GetLogicalDevice(void);
	VkFormat GetSupportedFormats(VkPhysicalDevice phyDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);
	bool isStencil(VkFormat format);
	bool GetInitStatus(void);

	void CreateImage(ui32 width, ui32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage& image, VkDeviceMemory& imageMemory);
	void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView);

	void BeginRecording(VkCommandBuffer& commandBuffer, VkCommandPool commandPool, VkCommandBufferUsageFlags commandBufferUsageFlags);
	void StopRecording(VkQueue queue, VkCommandBuffer commandBuffer, VkCommandPool commandPool);

	void ChangeLayout(VkCommandPool commandPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsageFlags, VkBuffer& buffer, VkMemoryPropertyFlags memoryFlags, VkDeviceMemory& memory);
	template <typename T>
	void CreateBufferOnGPU(std::vector<T> data, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory);
	void CreateDescriptorSets();

	VkCommandPool GetCommandPool();
	VkQueue GetGraphicsQueue();
private:
	void DrawFrame(void);
	void UpdateMVP(); //TODO

	void CreateInstance(const char* applicationName, ui32 applicationVersion);

	void CreateSurface(void);

	void CreatePhysicalDevice(void);
	void PickIdealPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices);

	void CreateLogicalDevice(void);

	void CreateSwapChain(void);
	void CreateImageViews(void);

	void CreateShaderModules(void);
	void CreateShaderModule(const std::vector<byte>& code, VkShaderModule* shaderModule);

	void CreateDescriptorSetLayout(void);
	void CreateDescriptorPool(void);
	void CreateDescriptorSet(Gameobject* gb, ui32 setIndex, bool instanced);

	void CreatePipeline(void);
	void CreateRenderpass(void);

	void CreateFramebuffers(void);

	void CreateCommandPool(void);
	void CreateCommandbuffer(void);

	void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

	void CreateObjectBuffers(VkBuffer& vertexBuffer, VkBuffer& indexBuffer, VkBuffer& uniformBuffer, VkDeviceMemory& vertexBufferMemory, VkDeviceMemory& indexBufferMemory, VkDeviceMemory& uniformBufferMemory, const char* meshName, bool instanced);

	void LoadModels(void);

	void CreateBuffersForObjects(void);

	void RecordCommands(void);
	void ReRecordCommands(void);

	void CreateSemaphores(void);

	void ExtractPlanes(Math::Mat4x4 mat);

	void CreateDepthImage(void);

	bool isFormatSupported(VkPhysicalDevice phyDevice, VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);
	bool isModeSupported(const std::vector<VkPresentModeKHR>& supportedPresentModes, VkPresentModeKHR presentMode);
	VkFormat GetSupportedSurfaceFormat(const std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats);
	VkColorSpaceKHR GetSupportedSurfaceColorSpace(const std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats);
	std::vector<byte> GetBuffer(RenderingBuffer bufferType);

	int maxInts = 2;
	int indexOfGraphicsQueue = INT_MAX;
	int indexOfPresentQueue = INT_MAX;
	int indexOfTransferQueue = INT_MAX;

	bool initialized = false;
	bool canGraphicsQueuePresent = false;

	std::vector<Gameobject*> gameObjects;

	VkInstance vkInstance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSurfaceKHR surface;

	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkQueue transferQueue = VK_NULL_HANDLE;
	ui16 queueCount = 0;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	std::vector<VkImageView> imageViews;
	std::vector<VkImage> swapchainImages;
	VkFormat supportedColorFormat;

	VkShaderModule vertexModule;
	VkShaderModule instanceModule;
	VkShaderModule fragmentModule;

	VkRenderPass renderpass;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkPipeline instancePipeline;

	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkViewport viewport;
	VkRect2D screenScissor;

	std::vector<VkFramebuffer> framebuffers;
	std::vector<VkPipelineStageFlags> waitStageMask;

	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSet> instancedDescriptorSets;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderingFinishedSemaphore;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;

	DepthImage depthImage;
	std::vector<std::string> texturePaths;
	std::vector<Model*> models;

	std::vector<std::vector<Gameobject*>> instancedObjects;

	std::vector<Vec4> planes;


	ui32 descriptorPoolSize = 0;
	ui32 gameObjectCount = 0;
};
