
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

class Gameobject;

class Rendering 
{
	DEFINE_SINGLETON(Rendering)

	enum class RenderingBuffer
	{
		VERTEX = 0,
		FRAGMENT = 1
	};

	struct alignas(16) VertexInputInfo
	{
		Math::Mat4x4 modelMatrix;
		Math::Mat4x4 viewMatrix;
		Math::Mat4x4 projectionMatrix;
		fColorRGBA color;
		fColorRGBA specColor;
		Math::Vec3 lightPosition;
		float ambientVal;
		float specularVal;
	};

public:
	void Initialize(const char* applicationName, ui32 applicationVersion);
	void Update(float time);
	void Cleanup(void);

	void RecreateSwapchain(void);
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsageFlags, VkBuffer& buffer, VkMemoryPropertyFlags memoryFlags, VkDeviceMemory& memory);
	
	ui32 FindMemoryTypeIndex(ui32 typeFilter, VkMemoryPropertyFlags properties);

	VkDevice GetLogicalDevice(void);
	VkFormat GetSupportedFormats(VkPhysicalDevice phyDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);
	bool isStencil(VkFormat format);
	bool GetInitStatus(void);

	void CreateImage(ui32 width, ui32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage& image, VkDeviceMemory& imageMemory);
	void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView);

	void BeginRecording(VkCommandBuffer& commandBuffer, VkCommandPool commandPool, VkCommandBufferUsageFlags commandBufferUsageFlags);
	void StopRecording(VkQueue queue, VkCommandBuffer commandBuffer, VkCommandPool commandPool);

	void ChangeLayout(VkCommandPool commandPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
private:
	void DrawFrame(void);
	void UpdateMVP(float time); //TODO

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
	void CreateDescriptorSet(void);

	void CreatePipeline(void);
	void CreateRenderpass(void);

	void CreateFramebuffers(void);

	void CreateCommandPool(void);
	void CreateCommandbuffer(void);

	void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

	template <typename T>
	void CreateBufferOnGPU(std::vector<T> data, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory);

	void CreateUniformbuffer(void);

	void LoadTextures(void);
	void LoadModels(void);

	void CreateInstanceData(void);
	void CreateInstanceBuffer(void);
	void CreateVertexbuffer(void);
	void CreateIndexbuffer(void);

	void RecordCommands(void);
	void ReRecordCommands(void);

	void CreateSemaphores(void);

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
	VkShaderModule fragmentModule;

	VkRenderPass renderpass;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkViewport viewport;
	VkRect2D screenScissor;

	std::vector<VkFramebuffer> framebuffers;
	std::vector<VkPipelineStageFlags> waitStageMask;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderingFinishedSemaphore;

	VkBuffer vertexBuffer;
	VkBuffer instanceBuffer;
	VkBuffer indexBuffer;
	VkBuffer uniformBuffer;

	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory instanceBufferMemory;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory uniformBufferMemory;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	Gameobject* root;
	Gameobject* testObject;

	Math::Vec3 cameraPos;

	std::vector<InstanceData> instanceData;

	std::vector<Texture*> textures;
	DepthImage depthImage;
	std::vector<Model*> models;
};
