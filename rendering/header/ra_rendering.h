
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
		INSTANCED = 1,
		FRAGMENT = 2,
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

	struct alignas(16) VertexInstancedInputInfo
	{
		Math::Mat4x4 modelMatrix[100];
		Math::Mat4x4 viewMatrix;
		Math::Mat4x4 projectionMatrix;
		fColorRGBA color[100];
		fColorRGBA specColor[100];
		Math::Vec3 lightPosition;
		float ambientVal;
		float specularVal;
	};

public:
	void Initialize(const char* applicationName, ui32 applicationVersion);
	void Update(float time);
	void Cleanup(void);

	void RecreateSwapchain(void);
	
	ui32 FindMemoryTypeIndex(ui32 typeFilter, VkMemoryPropertyFlags properties);

	std::vector<Vertex> GetVerticesOfObject(const char* name);
	std::vector<ui32> GetIndicesOfObject(const char* name);
	std::vector<Texture*> GetTextures();

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
	void CreateDescriptorSet(Gameobject* gb, ui32 setIndex, bool instanced);

	void CreatePipeline(void);
	void CreateRenderpass(void);

	void CreateFramebuffers(void);

	void CreateCommandPool(void);
	void CreateCommandbuffer(void);

	void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

	void CreateObjectBuffers(VkBuffer& vertexBuffer, VkBuffer& indexBuffer, VkBuffer& uniformBuffer, VkDeviceMemory& vertexBufferMemory, VkDeviceMemory& indexBufferMemory, VkDeviceMemory& uniformBufferMemory, const char* meshName, bool instanced);

	void LoadTextures(void);
	void LoadModels(void);

	void CreateBuffersForObjects(void);

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

	Math::Vec3 cameraPos;

	DepthImage depthImage;
	std::vector<Texture*> textures;
	std::vector<Model*> models;

	std::vector<std::vector<Gameobject*>> instancedObjects;

	ui32 renderableGameObjectsSize = 0;
};
