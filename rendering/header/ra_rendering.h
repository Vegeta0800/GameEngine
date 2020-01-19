
#pragma once
//EXTERNAL INCLUDES
#include <vector>
#include <chrono>
//INTERNAL INCLUDES
#include "math/ra_vector3.h"
#include "math/ra_mat4x4.h"
#include "ra_vertex.h"
#include "ra_types.h"
#include "ra_utils.h"
#include "ra_texture.h"
#include "ra_depthimage.h"
#include "ra_model.h"

class Gameobject;

class Rendering 
{
	DEFINE_SINGLETON(Rendering)

	//Different types of shader buffers
	enum class RenderingBuffer
	{
		VERTEX = 0,
		FRAGMENT = 1,
	};

	//Aligned vertex data that gets inputted into vertex shader
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

public:
	//Initialize Rendering class, setting up vulkan.
	void Initialize(const char* applicationName, ui32 applicationVersion);
	//Update function called every frame.
	void Update();
	//Cleanup Rendering class in inverted order.
	void Cleanup(void);

	//Create image using inputed information.
	void CreateImage(ui32 width, ui32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage& image, VkDeviceMemory& imageMemory);
	//Create image view using inputed information.
	void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView);
	
	//Create descriptorsets for all gameobjects
	void CreateDescriptorSets();

	//Create buffer on CPU side.
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsageFlags, VkBuffer& buffer, VkMemoryPropertyFlags memoryFlags, VkDeviceMemory& memory);
	//Create buffer on GPU side.
	template <typename T>
	void CreateBufferOnGPU(std::vector<T> data, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory);

	//Recreate swapchain on window resize. Destroy old buffers and swapchain. Then create them again.
	void RecreateSwapchain(void);
	
	//Change layout using inputed information.
	void ChangeLayout(VkCommandPool commandPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	
	//Begin recording of a command buffer using inputed information.
	void BeginRecording(VkCommandBuffer& commandBuffer, VkCommandPool commandPool, VkCommandBufferUsageFlags commandBufferUsageFlags);
	//Stop recording of a command buffer using inputed information.
	void StopRecording(VkQueue queue, VkCommandBuffer commandBuffer, VkCommandPool commandPool);

	//Get vertices of an object
	std::vector<Vertex> GetVerticesOfObject(std::string name);
	//Get indices of an object
	std::vector<ui32> GetIndicesOfObject(std::string name);

	//Get logical device.
	VkDevice GetLogicalDevice(void);
	//Get the optimal supported surface color format.
	VkFormat GetSupportedFormats(VkPhysicalDevice phyDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);
	//Get Command pool
	VkCommandPool GetCommandPool();
	//Get graphics queue
	VkQueue GetGraphicsQueue();
	
	//If supported find index of requested memory property.
	ui32 FindMemoryTypeIndex(ui32 typeFilter, VkMemoryPropertyFlags properties);
	
	//Check if stencil format is supported.
	bool isStencil(VkFormat format);
	//Get if rendering has finished initializing.
	bool GetInitStatus(void);

private:
	//Create vulkan instance with application infos, extensions and activated validation layers.
	void CreateInstance(const char* applicationName, ui32 applicationVersion);
	//Create surface (drawable region of the screen).
	void CreateSurface(void);
	//Get all physical devices in computer and execute PickIdealPhysicalDevice.
	void CreatePhysicalDevice(void);
	//Create logical device and get supported queues.
	void CreateLogicalDevice(void);

	//Create swapchain using surface information.
	void CreateSwapChain(void);
	//Create image views for swapchain.
	void CreateImageViews(void);

	//Create shader module using a code buffer.
	void CreateShaderModule(const std::vector<byte>& code, VkShaderModule* shaderModule);
	//Load shader data into byte buffers and create shader modules with them.
	void CreateShaderModules(void);

	//Create descriptorset layout using the descriptorset layout info.
	void CreateDescriptorSetLayout(void);
	//Create descriptorpool using the descriptorpool info.
	void CreateDescriptorPool(void);
	//Create descriptorset for a gameobject
	void CreateDescriptorSet(Gameobject* gb, ui32 setIndex);
		
	//Create all buffers for one object. A lot of references needed for this
	void CreateObjectBuffers(VkBuffer& vertexBuffer, VkBuffer& indexBuffer, VkBuffer& uniformBuffer, VkDeviceMemory& vertexBufferMemory, VkDeviceMemory& indexBufferMemory, VkDeviceMemory& uniformBufferMemory, const char* meshName, bool instanced);
	//Create all buffers for all objects
	void CreateBuffersForObjects(void);

	//Create pipeline using all the infos and stages defined in the pipeline info.
	void CreatePipeline(void);
	//Create renderpass using the declared dependencies and descriptions for the different representations.
	void CreateRenderpass(void);

	//Create Frame buffers using the frame buffer infos.
	void CreateFramebuffers(void);

	//Create Command pool
	void CreateCommandPool(void);
	//Allocate command buffer.
	void CreateCommandbuffer(void);

	//Create depth image.
	void CreateDepthImage(void);
	//Create semaphores.
	void CreateSemaphores(void);

	//Record commands for the graphics queue.
	void RecordCommands(void);
	//Recreate buffers, descriptor sets, and commands TODO
	void ReRecordCommands(void);

	//Update vertex shader with MVP and vertex data
	void UpdateMVP();
	//Draw frame
	void DrawFrame(void);
	
	//Rate physical devices on different factors and check for queues that are supported. Pick the best physical device.
	void PickIdealPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices);

	//Copy and transfer buffer to GPU side.
	void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

	//Load models.
	void LoadModels(void);

	//Get Buffer in bytes (unsigned char).
	std::vector<byte> GetBuffer(RenderingBuffer bufferType);

	//Get first supported format that fills the inputed requirements.
	VkFormat GetSupportedSurfaceFormat(const std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats);

	//Get the optimal supported surface color space.
	VkColorSpaceKHR GetSupportedSurfaceColorSpace(const std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats);
	
	//Check if a format is supported.
	bool isFormatSupported(VkPhysicalDevice phyDevice, VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);
	//Check if a present mode is supported.
	bool isModeSupported(const std::vector<VkPresentModeKHR>& supportedPresentModes, VkPresentModeKHR presentMode);


	//Variables
	std::vector<Gameobject*> gameObjects;
	std::vector<std::string> texturePaths;
	std::vector<Model*> models;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkPipelineStageFlags> waitStageMask;

	DepthImage depthImage;

	VkInstance vkInstance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSurfaceKHR surface;

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;

	VkRenderPass renderpass;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	VkCommandPool m_commandPool;
	
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkQueue transferQueue = VK_NULL_HANDLE;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;

	VkShaderModule vertexModule;
	VkShaderModule fragmentModule;

	VkViewport viewport;
	VkRect2D screenScissor;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderingFinishedSemaphore;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;

	VkFormat supportedColorFormat;

	ui32 descriptorPoolSize = 0;
	ui32 gameObjectCount = 0;
	ui16 queueCount = 0;

	int indexOfGraphicsQueue = INT_MAX;
	int indexOfPresentQueue = INT_MAX;
	int indexOfTransferQueue = INT_MAX;

	bool initialized = false;
	bool canGraphicsQueuePresent = false;
};
