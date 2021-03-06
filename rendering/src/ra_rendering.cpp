
#define VK_USE_PLATFORM_WIN32_KHR //Define to use vulkan with windows
// EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
#include <list>
// INTERNAL INCLUDES
#include "ra_rendering.h"
#include "filesystem/ra_filesystem.h"
#include "filesystem/ra_winfile.h"
#include "math/ra_mathfunctions.h"
#include "math/ra_mat4x4.h"
#include "ra_model.h"
#include "ra_mesh.h"
#include "ra_material.h"
#include "ra_depthimage.h"
#include "ra_application.h"
#include "ra_window.h"
#include "ra_scenemanager.h"
#include "ra_scene.h"
#include "ra_camera.h"
#include "ra_gameobject.h"
#include "ra_types.h"
#include "ra_utils.h"
#include "ra_vkcheck.h"

//Singleton
DECLARE_SINGLETON(Rendering)

///Public Functions

//Initialize Rendering class, setting up vulkan.
void Rendering::Initialize(const char* applicationName, ui32 applicationVersion)
{
	//Create Instance
	this->CreateInstance(applicationName, applicationVersion);
	//Create surface from window handle
	this->CreateSurface();

	//Create devices
	this->CreatePhysicalDevice();
	this->CreateLogicalDevice();

	//Create swapchain
	this->CreateSwapChain();
	//Create image views
	this->CreateImageViews();

	//Create renderpass
	this->CreateRenderpass();
	//Create descriptorset layout
	this->CreateDescriptorSetLayout();
	//Create pipeline
	this->CreatePipeline();

	//Create command pool
	this->CreateCommandPool();

	//Create depth image
	this->CreateDepthImage();
	//Create frame buffer
	this->CreateFramebuffers();
	
	//Load all models
	this->LoadModels();

	//Create all buffers for all renderable objects
	this->CreateBuffersForObjects();

	//Create descriptor pool
	this->CreateDescriptorPool();
	//Create descriptor sets for all objects
	this->CreateDescriptorSets();

	//Create command buffer
	this->CreateCommandbuffer();
	//Record commands
	this->RecordCommands();
	//Create semaphores
	this->CreateSemaphores();

	//Set aspect ratio
	Application::GetInstancePtr()->GetAspectRatio() = static_cast<float>(this->surfaceCapabilities.currentExtent.width) / static_cast<float>(this->surfaceCapabilities.currentExtent.height);

	//Set initial gameobject count
	this->gameObjectCount = SceneManager::GetInstancePtr()->GetActiveScene()->GetObjectCount();

	//Set window state to start and show main window
	Window::GetInstancePtr()->SetState(Window::WindowState::Started);
	Window::GetInstancePtr()->ShowActiveWindow();

	//Finished initializing.
	this->initialized = true;
}
//Update function called every frame.
void Rendering::Update()
{
	//If there is a new object
	if (SceneManager::GetInstancePtr()->GetActiveScene()->GetObjectCount() != this->gameObjectCount)
	{
		//Adjust game object count
		this->gameObjectCount = SceneManager::GetInstancePtr()->GetActiveScene()->GetObjectCount();
		//Re record commands
		this->ReRecordCommands();
	}

	//Update shader values
	this->UpdateMVP();
	//Draw frame
	this->DrawFrame();
}
//Cleanup Rendering class in inverted order.
void Rendering::Cleanup()
{
	//Wait for the device idle.
	VK_CHECK(vkDeviceWaitIdle(this->logicalDevice));

	//Destroy semaphores and clear waitstage mask.
	this->waitStageMask.clear();
	vkDestroySemaphore(this->logicalDevice, this->imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(this->logicalDevice, this->renderingFinishedSemaphore, nullptr);

	//Destroy depth image.
	this->depthImage.Destroy();

	//Destroy descriptorSet layout and descriptorpool.
	if (this->descriptorSets.size() > 0)
	{
		vkFreeDescriptorSets(this->logicalDevice, this->descriptorPool, static_cast<ui32>(this->descriptorSets.size()), this->descriptorSets.data());
		this->descriptorSets.clear();
	}
	vkDestroyDescriptorPool(this->logicalDevice, this->descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(this->logicalDevice, this->descriptorSetLayout, nullptr);

	Scene* scene = SceneManager::GetInstancePtr()->GetActiveScene();

	//Free vertex, index and uniform buffers
	for (Gameobject* gb : scene->GetAllGameobjects())
	{
		vkDestroyBuffer(this->logicalDevice, scene->GetMesh(gb->GetName())->GetIndexBuffer(), nullptr);
		vkFreeMemory(this->logicalDevice, scene->GetMesh(gb->GetName())->GetIndexBufferMem(), nullptr);

		vkDestroyBuffer(this->logicalDevice, scene->GetMesh(gb->GetName())->GetVertexBuffer(), nullptr);
		vkFreeMemory(this->logicalDevice, scene->GetMesh(gb->GetName())->GetVertexBufferMem(), nullptr);

		vkDestroyBuffer(this->logicalDevice, scene->GetMesh(gb->GetName())->GetUniformBuffer(), nullptr);
		vkFreeMemory(this->logicalDevice, scene->GetMesh(gb->GetName())->GetUniformBufferMem(), nullptr);

		//Cleanup mesh
		scene->GetMesh(gb->GetName())->Cleanup();

		//Release all textures
		for(Texture* tex : scene->GetMesh(gb->GetName())->GetTextures())
		{
			tex->Release();
		}
	}

	//Delete models
	for (Model* model : this->models)
	{
		delete model;
	}


	//Free memory of command buffers.
	vkFreeCommandBuffers(this->logicalDevice, this->m_commandPool, static_cast<ui32>(this->commandBuffers.size()), this->commandBuffers.data());
	
	//Destroy of command pool.
	vkDestroyCommandPool(this->logicalDevice, this->m_commandPool, nullptr);

	//Destroy all frame buffers.
	for (size_t i = 0; i < this->framebuffers.size(); i++)
	{
		vkDestroyFramebuffer(this->logicalDevice, this->framebuffers[i], nullptr);
	}
	this->framebuffers.clear();
	
	//Destroy all image views.
	for (size_t i = 0; i < this->imageViews.size(); i++)
	{
		vkDestroyImageView(this->logicalDevice, this->imageViews[i], nullptr);
	}
	this->imageViews.clear();

	//Clear swapchain images.
	this->swapchainImages.clear();

	//Destroy pipeline.
	vkDestroyPipeline(this->logicalDevice, this->pipeline, nullptr);

	//Destroy pipeline layout.
	vkDestroyPipelineLayout(this->logicalDevice, this->pipelineLayout, nullptr);

	//Destroy renderpass.
	vkDestroyRenderPass(this->logicalDevice, this->renderpass, nullptr);

	//Destroy shadermodules.
	vkDestroyShaderModule(this->logicalDevice, this->vertexModule, nullptr);
	vkDestroyShaderModule(this->logicalDevice, this->fragmentModule, nullptr);

	//Destroy swapchain.
	vkDestroySwapchainKHR(this->logicalDevice, this->swapchain, nullptr);

	//Destroy device.
	vkDestroyDevice(this->logicalDevice, nullptr);

	//Destroy surface.
	vkDestroySurfaceKHR(this->vkInstance, this->surface, nullptr);

	//Destroy instance.
	vkDestroyInstance(this->vkInstance, nullptr);
} 

//Create image using inputed information.
void Rendering::CreateImage(ui32 width, ui32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage & image, VkDeviceMemory & imageMemory)
{
	//Image info using the inputed parameters.
	VkImageCreateInfo imageInfo;
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = nullptr;
	imageInfo.flags = 0;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = format;
	imageInfo.extent = { width, height, 1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = tiling;
	imageInfo.usage = usageFlags;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	//Create the image.
	VK_CHECK(vkCreateImage(this->logicalDevice, &imageInfo, nullptr, &image));

	//Get memory requirements and allocate the memory.
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(this->logicalDevice, image, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = Rendering::GetInstancePtr()->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, propertyFlags);

	VK_CHECK(vkAllocateMemory(this->logicalDevice, &memoryAllocateInfo, nullptr, &imageMemory));

	//Bind image to allocated memory.
	vkBindImageMemory(this->logicalDevice, image, imageMemory, 0);
}
//Create image view using inputed information.
void Rendering::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView)
{
	//Create image view using inputed information.
	VkImageViewCreateInfo imageViewInfo;
	imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.pNext = nullptr;
	imageViewInfo.flags = 0;
	imageViewInfo.image = image;
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.format = format;
	imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.subresourceRange.aspectMask = aspectFlags;
	imageViewInfo.subresourceRange.baseMipLevel = 0;
	imageViewInfo.subresourceRange.levelCount = 1;
	imageViewInfo.subresourceRange.baseArrayLayer = 0;
	imageViewInfo.subresourceRange.layerCount = 1;

	VK_CHECK(vkCreateImageView(this->logicalDevice, &imageViewInfo, nullptr, &imageView));
}

//Create descriptorsets for all gameobjects
void Rendering::CreateDescriptorSets()
{
	std::vector<Gameobject*> tempGbs;
	std::vector<VkDescriptorSetLayout> layouts;

	//Get how many sets are needed
	for (Gameobject* gb : SceneManager::GetInstancePtr()->GetActiveScene()->GetAllGameobjects())
	{
		if (gb->GetIsRenderable() && gb->GetIsActive())
		{
			tempGbs.push_back(gb);
			layouts.push_back(this->descriptorSetLayout);
		}
	}
	this->descriptorSets.resize(tempGbs.size());

	//Create descriptor allocate info for all of them
	if (this->descriptorSets.size() > 0)
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = this->descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = 1;

		for (ui32 i = 0; i < this->descriptorSets.size(); i++)
		{
			//Allocate space for descriptorset using the descriptorpool and descriptorsetlayout.
			descriptorSetAllocateInfo.pSetLayouts = &layouts[i];
			vkAllocateDescriptorSets(this->logicalDevice, &descriptorSetAllocateInfo, &this->descriptorSets[i]);
		}
	}

	//Create the descriptor sets
	for (ui32 i = 0; i < tempGbs.size(); i++)
	{
		if (tempGbs[i]->GetIsRenderable() && tempGbs[i]->GetIsActive())
		{
			this->CreateDescriptorSet(tempGbs[i], i);
		}
	}
}

//Create buffer on CPU side.
void Rendering::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsageFlags, VkBuffer& buffer, VkMemoryPropertyFlags memoryFlags, VkDeviceMemory& memory)
{
	//Buffer info using the size and usage flags.
	VkBufferCreateInfo bufferInfo;
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.flags = 0;
	bufferInfo.size = size;
	bufferInfo.usage = bufferUsageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.queueFamilyIndexCount = 0;
	bufferInfo.pQueueFamilyIndices = nullptr;

	//Create buffer using the buffer info.
	vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &buffer);

	//Get Memory needed for storing the buffer and allocate that memory.
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(this->logicalDevice, buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = this->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryFlags);

	vkAllocateMemory(this->logicalDevice, &memoryAllocateInfo, nullptr, &memory);

	//Bind the allocated memory to the buffer.
	vkBindBufferMemory(this->logicalDevice, buffer, memory, 0);
}
//Create buffer on GPU side.
template <typename T>
void Rendering::CreateBufferOnGPU(std::vector<T> data, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory)
{
	//Get needed size of buffer.
	VkDeviceSize bufferSize = sizeof(T) * data.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	//Create staging buffer with specifications for transfering data.
	this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), stagingBufferMemory);

	//Map memory, copy data to staging buffers memory, unmap memory.
	void* pointerData;
	vkMapMemory(this->logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &pointerData);
	memcpy(pointerData, data.data(), bufferSize);
	vkUnmapMemory(this->logicalDevice, stagingBufferMemory);

	//Create actual buffer.
	this->CreateBuffer(bufferSize, (usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT), buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory);

	//Copy staging buffer info to actual buffer.
	this->CopyBuffer(stagingBuffer, buffer, bufferSize);

	//Destroy staging buffer and free the allocated memory.
	vkDestroyBuffer(this->logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(this->logicalDevice, stagingBufferMemory, nullptr);
}

//Recreate swapchain on window resize. Destroy old buffers and swapchain. Then create them again.
void Rendering::RecreateSwapchain()
{
	//Wait for idle of logical device.
	vkDeviceWaitIdle(this->logicalDevice);

	//Destroy depth image
	this->depthImage.Destroy();
	
	//Free command buffer
	vkFreeCommandBuffers(this->logicalDevice, this->m_commandPool, static_cast<ui32>(this->commandBuffers.size()), this->commandBuffers.data());

	//Destroy frame buffers
	for (size_t i = 0; i < this->framebuffers.size(); i++)
	{
		vkDestroyFramebuffer(this->logicalDevice, this->framebuffers[i], nullptr);
	}
	this->framebuffers.clear();

	//Destroy image views
	for (size_t i = 0; i < this->imageViews.size(); i++)
	{
		vkDestroyImageView(this->logicalDevice, this->imageViews[i], nullptr);
	}
	this->imageViews.clear();
	this->swapchainImages.clear();

	//Destroy render pass
	vkDestroyRenderPass(this->logicalDevice, this->renderpass, nullptr);
	
	//Save current swapchain
	VkSwapchainKHR oldSwapchain = this->swapchain;

	//Recreate everything necessary.
	this->CreateSwapChain();
	this->CreateImageViews();
	this->CreateRenderpass();
	this->CreateDepthImage();
	this->CreateFramebuffers();
	this->CreateCommandbuffer();
	this->RecordCommands();

	//Destroy old swapchain
	vkDestroySwapchainKHR(this->logicalDevice, oldSwapchain, nullptr);

	//Adjust aspect ratio
	Application::GetInstancePtr()->GetAspectRatio() = static_cast<float>(this->surfaceCapabilities.currentExtent.width) / static_cast<float>(this->surfaceCapabilities.currentExtent.height);
}

//Change layout using inputed information.
void Rendering::ChangeLayout(VkCommandPool commandPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	//Begin recording with new command buffer.
	this->BeginRecording(commandBuffer, commandPool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Image memory barrier (blocks queues requesting the image while being changed).
	VkImageMemoryBarrier imageMemoryBarrier = { };
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	//First time being called change to transfer data. Next time change to let shader read information.
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	//For depth buffer and stencil buffer.
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
		throw;

	imageMemoryBarrier.oldLayout = oldLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	
	//Change image to present depth and/or stencil if required.
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (Rendering::GetInstancePtr()->isStencil(format))
		{
			imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	//Put barrier inside the pipeline at the beginning.
	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	//Stop recording of command buffer.
	this->StopRecording(queue, commandBuffer, commandPool);
}

//Begin recording of a command buffer using inputed information.
void Rendering::BeginRecording(VkCommandBuffer& commandBuffer, VkCommandPool commandPool, VkCommandBufferUsageFlags commandBufferUsageFlags)
{
	//Allocate inputed buffer.
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;

	VK_CHECK(vkAllocateCommandBuffers(this->logicalDevice, &commandBufferAllocateInfo, &commandBuffer));

	//Begin command buffer recording.
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = commandBufferUsageFlags;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));
}
//Stop recording of a command buffer using inputed information.
void Rendering::StopRecording(VkQueue queue, VkCommandBuffer commandBuffer, VkCommandPool commandPool)
{
	//End command buffer recording.
	vkEndCommandBuffer(commandBuffer);

	//Standard submit info.
	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	//Submit to inputed queue.
	VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
	vkQueueWaitIdle(queue);

	//After waiting for inputed queues idle, free the command buffer.
	vkFreeCommandBuffers(this->logicalDevice, commandPool, 1, &commandBuffer);
}

//Get vertices of an object
std::vector<Vertex> Rendering::GetVerticesOfObject(std::string name)
{
	for (ui32 i = 0; i < this->models.size(); i++)
	{
		//Find associated model and return vertices
		std::string stringName(name);
		if (this->models[i]->GetName() == stringName)
			return this->models[i]->GetVertices();
	}

	return std::vector<Vertex>();
}
//Get indices of an object
std::vector<ui32> Rendering::GetIndicesOfObject(std::string name)
{
	for (Model* model : this->models)
	{
		//Find associated model and return indices
		std::string stringName(name);
		if (model->GetName() == stringName)
			return model->GetIndices();
	}

	return std::vector<ui32>();
}

//Get logical device.
VkDevice Rendering::GetLogicalDevice(void)
{
	return this->logicalDevice;
}
//Get the optimal supported surface color format.
VkFormat Rendering::GetSupportedSurfaceFormat(const std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats)
{
	for (ui16 i = 0; i < supportedSurfaceFormats.size(); i++)
	{
		if (supportedSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
			return VK_FORMAT_B8G8R8A8_UNORM;
	}

	return supportedSurfaceFormats[0].format;
}
//Get Command pool
VkCommandPool Rendering::GetCommandPool()
{
	return this->m_commandPool;
}
//Get graphics queue
VkQueue Rendering::GetGraphicsQueue()
{
	return this->graphicsQueue;
}

//If supported find index of requested memory property.
ui32 Rendering::FindMemoryTypeIndex(ui32 typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &deviceMemoryProperties);

	for (ui32 i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	return 0;
}

//Check if stencil format is supported.
bool Rendering::isStencil(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
//Get if rendering has finished initializing.
bool Rendering::GetInitStatus()
{
	return this->initialized;
}


///Private Functions


//Create vulkan instance with application infos, extensions and activated validation layers.
void Rendering::CreateInstance(const char* applicationName, ui32 applicationVersion)
{
	//Define validation layers for everything (from instance).
	const std::vector<const char*> instanceValidationLayers =
	{
		"VK_LAYER_LUNARG_standard_validation",
	};

	//Define extensions for everything (from instance).
	const std::vector<const char*> instanceExtensions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	};

	//Application info that stores information about the application.
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = applicationName;
	appInfo.applicationVersion = applicationVersion;
	appInfo.pEngineName = "RA Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0); // TODO version
	appInfo.apiVersion = VK_API_VERSION_1_1;

	//Instance info with needed extensions and the application info. Activate validation layers if in debug mode.
	VkInstanceCreateInfo instanceInfo = { };
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = static_cast<ui32>(instanceExtensions.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

#if _DEBUG
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = nullptr;
#else
	instanceInfo.enabledLayerCount = static_cast<ui32>(instanceValidationLayers.size());
	instanceInfo.ppEnabledLayerNames = instanceValidationLayers.data();
#endif

	//Create instance with instance info.
	VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &this->vkInstance));
}
//Create surface (drawable region of the screen).
void Rendering::CreateSurface()
{
	//Win32 surface info with the the handle of the window.
	VkWin32SurfaceCreateInfoKHR surfaceInfo;
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0;
	surfaceInfo.hinstance = GetModuleHandle(NULL);
	surfaceInfo.hwnd = Window::GetInstancePtr()->GetHandle();

	//Create Win32 surface with surface infos using the instance.
	VK_CHECK(vkCreateWin32SurfaceKHR(this->vkInstance, &surfaceInfo, nullptr, &this->surface));
}
//Get all physical devices in computer and execute PickIdealPhysicalDevice.
void Rendering::CreatePhysicalDevice()
{
	//Get amount of physical devices in computer
	ui32 physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(this->vkInstance, &physicalDeviceCount, nullptr);

	//Get phyiscal devices.
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(this->vkInstance, &physicalDeviceCount, physicalDevices.data()));

	//Pick the device.
	PickIdealPhysicalDevice(physicalDevices);
}
//Create logical device and get supported queues.
void Rendering::CreateLogicalDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queueInfos;

	//Create queue infos for logical device with the same priority.
	float queuePriority = 1.0f;
	for (ui16 i = 0; i < this->queueCount; i++)
	{
		VkDeviceQueueCreateInfo queueInfo;
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.pNext = nullptr;
		queueInfo.flags = 0;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &queuePriority;
		if (i == static_cast<ui16>(this->indexOfGraphicsQueue))
			queueInfo.queueFamilyIndex = i;
		else if (i == static_cast<ui16>(this->indexOfPresentQueue))
			queueInfo.queueFamilyIndex = i;
		else if (i == static_cast<ui16>(this->indexOfTransferQueue))
			queueInfo.queueFamilyIndex = i;

		queueInfos.push_back(queueInfo);
	}

	//Set all features to false and enable the features needed.
	VkPhysicalDeviceFeatures enabledFeatures = { };
	enabledFeatures.samplerAnisotropy = VK_TRUE;

	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	//Create logical device info.
	VkDeviceCreateInfo deviceInfo;
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.flags = 0;
	deviceInfo.queueCreateInfoCount = static_cast<ui32>(queueInfos.size());
	deviceInfo.pQueueCreateInfos = queueInfos.data();
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledLayerNames = nullptr;
	deviceInfo.enabledExtensionCount = static_cast<ui32>(deviceExtensions.size());
	deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
	deviceInfo.pEnabledFeatures = &enabledFeatures;

	//Create logical device
	VK_CHECK(vkCreateDevice(this->physicalDevice, &deviceInfo, nullptr, &this->logicalDevice));

	//Get queues if supported.
	for (ui16 i = 0; i < this->queueCount; i++)
	{
		if (i == static_cast<ui16>(this->indexOfGraphicsQueue))
			vkGetDeviceQueue(this->logicalDevice, i, 0, &this->graphicsQueue);
		else if (i == static_cast<ui16>(this->indexOfPresentQueue))
			vkGetDeviceQueue(this->logicalDevice, i, 0, &this->presentQueue);
		else if (i == static_cast<ui16>(this->indexOfTransferQueue))
			vkGetDeviceQueue(this->logicalDevice, i, 0, &this->transferQueue);
	}
}

//Create swapchain using surface information.
void Rendering::CreateSwapChain(void)
{
	//Get surface capabilities.
	this->surfaceCapabilities;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDevice, this->surface, &this->surfaceCapabilities));

	//Get supported formats of the physical device.
	ui32 formatsCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, this->surface, &formatsCount, nullptr);
	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, this->surface, &formatsCount, surfaceFormats.data()));

	//Get supported presentation modes of the physical device.
	ui32 presentationModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->physicalDevice, this->surface, &presentationModeCount, nullptr);
	std::vector<VkPresentModeKHR> presentModes(presentationModeCount);
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(this->physicalDevice, this->surface, &presentationModeCount, presentModes.data()));

	//Get supported surface color format.
	this->supportedColorFormat = this->GetSupportedSurfaceFormat(surfaceFormats);

	//Swapchain info with infos on surface capabilities, present modes and surface formats.
	VkSwapchainCreateInfoKHR swapchainInfo;
	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.pNext = nullptr;
	swapchainInfo.flags = 0;
	swapchainInfo.surface = this->surface;
	swapchainInfo.minImageCount = this->surfaceCapabilities.maxImageCount >= 3 ? 3 : 2; //If triple buffering is supported activate it. Else activate double buffering.
	swapchainInfo.imageFormat = this->supportedColorFormat;
	swapchainInfo.imageColorSpace = this->GetSupportedSurfaceColorSpace(surfaceFormats); //Get supported surface color space.
	swapchainInfo.imageExtent.height = this->surfaceCapabilities.currentExtent.height; //Image size from swapchain.
	swapchainInfo.imageExtent.width = this->surfaceCapabilities.currentExtent.width; //Image size from swapchain.
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //Change if swapchain should be shared with different queues.
	swapchainInfo.queueFamilyIndexCount = 0; //Change if swapchain should be shared with different queues.
	swapchainInfo.pQueueFamilyIndices = nullptr; //Change if swapchain should be shared with different queues.
	swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR; //Identity transform.
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //Standard mode available.
	swapchainInfo.presentMode = this->isModeSupported(presentModes, VK_PRESENT_MODE_MAILBOX_KHR) ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR; //If Mailbox is supported tkae mailbox (override image if already new data available). Else take standard mode.
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = this->swapchain; //Important for recreating swapchain.

	//Create swapchain with swapchain info.
	VK_CHECK(vkCreateSwapchainKHR(this->logicalDevice, &swapchainInfo, nullptr, &this->swapchain));
}
//Create image views for swapchain.
void Rendering::CreateImageViews(void)
{
	//Get swapchain images.
	ui32 swapchainImageCount = 0;
	vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &swapchainImageCount, nullptr);
	
	this->swapchainImages.resize(swapchainImageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &swapchainImageCount, this->swapchainImages.data()));

	this->imageViews.resize(swapchainImageCount);

	//Create image view for every image in swapchain.
	for (ui32 i = 0; i < swapchainImageCount; i++)
	{
		this->CreateImageView(this->swapchainImages[i], this->supportedColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, this->imageViews[i]);
	}
}

//Create shader module using a code buffer.
void Rendering::CreateShaderModule(const std::vector<byte>& code, VkShaderModule* shaderModule)
{
	//Shader info using the shader code in a buffer.
	VkShaderModuleCreateInfo shaderInfo;
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.pNext = nullptr;
	shaderInfo.flags = 0;
	shaderInfo.codeSize = code.size() % 4 + code.size(); //Size in bytes. If size isnt divisible by 4 (1 byte) then add the rest to secure no data is lost.
	shaderInfo.pCode = (ui32*)code.data();

	//Create shader module.
	VK_CHECK(vkCreateShaderModule(this->logicalDevice, &shaderInfo, nullptr, shaderModule));
}
//Load shader data into byte buffers and create shader modules with them.
void Rendering::CreateShaderModules()
{
	//Get buffers
	std::vector<byte> vertexCode = GetBuffer(RenderingBuffer::VERTEX);
	std::vector<byte> fragmentCode = GetBuffer(RenderingBuffer::FRAGMENT);

	//Create shader modules.
	this->CreateShaderModule(vertexCode, &this->vertexModule);
	this->CreateShaderModule(fragmentCode, &this->fragmentModule);
}

//Create descriptorset layout using the descriptorset layout info.
void Rendering::CreateDescriptorSetLayout()
{
	//Store uniform data bindings of shaders.
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

	//Vertex binding (mvp matrix).
	VkDescriptorSetLayoutBinding vertexSetLayoutBinding;
	vertexSetLayoutBinding.binding = 0;
	vertexSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vertexSetLayoutBinding.descriptorCount = 1;
	vertexSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vertexSetLayoutBinding.pImmutableSamplers = nullptr;
	descriptorSetLayoutBindings.push_back(vertexSetLayoutBinding);

	//Sampler binding for fragment shader (texcoords).
	VkDescriptorSetLayoutBinding samplerSetLayoutBinding;
	samplerSetLayoutBinding.binding = 1;
	samplerSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerSetLayoutBinding.descriptorCount = 1;
	samplerSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerSetLayoutBinding.pImmutableSamplers = nullptr;
	descriptorSetLayoutBindings.push_back(samplerSetLayoutBinding);

	//Sampler binding for fragment shader (texcoords).
	VkDescriptorSetLayoutBinding samplerNormalMapSetLayoutBinding;
	samplerNormalMapSetLayoutBinding.binding = 2;
	samplerNormalMapSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerNormalMapSetLayoutBinding.descriptorCount = 1;
	samplerNormalMapSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerNormalMapSetLayoutBinding.pImmutableSamplers = nullptr;
	descriptorSetLayoutBindings.push_back(samplerNormalMapSetLayoutBinding);

	//Sampler binding for fragment shader (texcoords).
	VkDescriptorSetLayoutBinding samplerEmissionMapSetLayoutBinding;
	samplerEmissionMapSetLayoutBinding.binding = 3;
	samplerEmissionMapSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerEmissionMapSetLayoutBinding.descriptorCount = 1;
	samplerEmissionMapSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerEmissionMapSetLayoutBinding.pImmutableSamplers = nullptr;
	descriptorSetLayoutBindings.push_back(samplerEmissionMapSetLayoutBinding);

	//Sampler binding for fragment shader (texcoords).
	VkDescriptorSetLayoutBinding samplerRoughnessMapSetLayoutBinding;
	samplerRoughnessMapSetLayoutBinding.binding = 4;
	samplerRoughnessMapSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerRoughnessMapSetLayoutBinding.descriptorCount = 1;
	samplerRoughnessMapSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerRoughnessMapSetLayoutBinding.pImmutableSamplers = nullptr;
	descriptorSetLayoutBindings.push_back(samplerRoughnessMapSetLayoutBinding);

	//Sampler binding for fragment shader (texcoords).
	VkDescriptorSetLayoutBinding samplerAmbientMapSetLayoutBinding;
	samplerAmbientMapSetLayoutBinding.binding = 5;
	samplerAmbientMapSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerAmbientMapSetLayoutBinding.descriptorCount = 1;
	samplerAmbientMapSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerAmbientMapSetLayoutBinding.pImmutableSamplers = nullptr;
	descriptorSetLayoutBindings.push_back(samplerAmbientMapSetLayoutBinding);

	//Descriptorset layout info using the bindings.
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo;
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.pNext = nullptr;
	descriptorSetLayoutInfo.flags = 0;
	descriptorSetLayoutInfo.bindingCount = static_cast<ui32>(descriptorSetLayoutBindings.size());
	descriptorSetLayoutInfo.pBindings = descriptorSetLayoutBindings.data();

	//Create descriptorset layout using the descriptorset layout info.
	VK_CHECK(vkCreateDescriptorSetLayout(this->logicalDevice, &descriptorSetLayoutInfo, nullptr, &this->descriptorSetLayout));
}
//Create descriptorpool using the descriptorpool info.
void Rendering::CreateDescriptorPool()
{
	//Store pool size for uniform data needed in shaders.
	std::vector<VkDescriptorPoolSize> descriptorPoolSizes;

	//Uniform buffer pool size in vertex shader (mvp matrix).
	VkDescriptorPoolSize uniformPoolSize;
	uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformPoolSize.descriptorCount = 1;
	descriptorPoolSizes.push_back(uniformPoolSize);

	//Sampler buffer pool size in fragment shader (texcoords).
	VkDescriptorPoolSize samplerPoolSize;
	samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerPoolSize.descriptorCount = 1;
	descriptorPoolSizes.push_back(samplerPoolSize);

	//Sampler buffer pool size in fragment shader (texcoords).
	VkDescriptorPoolSize samplerNormalMapPoolSize;
	samplerNormalMapPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerNormalMapPoolSize.descriptorCount = 1;
	descriptorPoolSizes.push_back(samplerNormalMapPoolSize);

	//Sampler buffer pool size in fragment shader (texcoords).
	VkDescriptorPoolSize samplerEmissionMapPoolSize;
	samplerEmissionMapPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerEmissionMapPoolSize.descriptorCount = 1;
	descriptorPoolSizes.push_back(samplerEmissionMapPoolSize);

	//Sampler buffer pool size in fragment shader (texcoords).
	VkDescriptorPoolSize samplerRoughnessMapPoolSize;
	samplerRoughnessMapPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerRoughnessMapPoolSize.descriptorCount = 1;
	descriptorPoolSizes.push_back(samplerRoughnessMapPoolSize);

	//Sampler buffer pool size in fragment shader (texcoords).
	VkDescriptorPoolSize samplerAmbientMapPoolSize;
	samplerAmbientMapPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerAmbientMapPoolSize.descriptorCount = 1;
	descriptorPoolSizes.push_back(samplerAmbientMapPoolSize);

	//Get how many descriptor pools are needed
	for (Gameobject* gb : SceneManager::GetInstancePtr()->GetActiveScene()->GetAllGameobjects())
	{
		if (gb->GetIsRenderable() && gb->GetIsActive())
		{
			this->descriptorPoolSize++;
		}
	}

	//Descriptorpool info using pool sizes.
	VkDescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.pNext = nullptr;
	descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolInfo.maxSets = descriptorPoolSize;
	descriptorPoolInfo.poolSizeCount = static_cast<ui32>(descriptorPoolSizes.size());
	descriptorPoolInfo.pPoolSizes = descriptorPoolSizes.data();

	//Create descriptorpool using the descriptorpool info.
	vkCreateDescriptorPool(this->logicalDevice, &descriptorPoolInfo, nullptr, &this->descriptorPool);
}
//Create descriptorset for a gameobject
void Rendering::CreateDescriptorSet(Gameobject* gb, ui32 setIndex)
{
	//Get mesh from gameobject
	Mesh* mesh = SceneManager::GetInstancePtr()->GetActiveScene()->GetMesh(gb->GetName());

	std::vector<VkWriteDescriptorSet> descriptorSetWrites;
	//Uniform buffer info (for mvp matrix and fragInfos in vertex shader).
	VkDescriptorBufferInfo uniformDescriptorBufferInfo;
	uniformDescriptorBufferInfo.buffer = mesh->GetUniformBuffer();
	uniformDescriptorBufferInfo.offset = 0;

	uniformDescriptorBufferInfo.range = sizeof(VertexInputInfo);

	//Uniform buffer write using uniformDescriptorBufferInfo.
	VkWriteDescriptorSet uniformDescriptorWrite;
	uniformDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	uniformDescriptorWrite.pNext = nullptr;
	uniformDescriptorWrite.dstSet = this->descriptorSets[setIndex];
	uniformDescriptorWrite.dstBinding = 0;
	uniformDescriptorWrite.dstArrayElement = 0;
	uniformDescriptorWrite.descriptorCount = 1;
	uniformDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformDescriptorWrite.pImageInfo = nullptr;
	uniformDescriptorWrite.pBufferInfo = &uniformDescriptorBufferInfo;
	uniformDescriptorWrite.pTexelBufferView = nullptr;
	descriptorSetWrites.push_back(uniformDescriptorWrite);

	//Sampler info (for texcoords in fragment shader).
	VkDescriptorImageInfo descriptorSamplerInfo;
	descriptorSamplerInfo.sampler = mesh->GetTextures()[0]->GetSampler();
	descriptorSamplerInfo.imageView = mesh->GetTextures()[0]->GetImageView();
	descriptorSamplerInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Sampler write using descriptorSamplerInfo.
	VkWriteDescriptorSet descriptorSamplerWrite;
	descriptorSamplerWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorSamplerWrite.pNext = nullptr;
	descriptorSamplerWrite.dstSet = this->descriptorSets[setIndex];
	descriptorSamplerWrite.dstBinding = 1;
	descriptorSamplerWrite.dstArrayElement = 0;
	descriptorSamplerWrite.descriptorCount = 1;
	descriptorSamplerWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSamplerWrite.pImageInfo = &descriptorSamplerInfo;
	descriptorSamplerWrite.pBufferInfo = nullptr;
	descriptorSamplerWrite.pTexelBufferView = nullptr;
	descriptorSetWrites.push_back(descriptorSamplerWrite);

	//Sampler info (for texcoords normalMap in fragment shader).
	VkDescriptorImageInfo descriptorNormalMapInfo;
	descriptorNormalMapInfo.sampler = mesh->GetTextures()[1]->GetSampler();
	descriptorNormalMapInfo.imageView = mesh->GetTextures()[1]->GetImageView();
	descriptorNormalMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Sampler write using descriptorSamplerInfo.
	VkWriteDescriptorSet descriptorNormalMapWrite;
	descriptorNormalMapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorNormalMapWrite.pNext = nullptr;
	descriptorNormalMapWrite.dstSet = this->descriptorSets[setIndex];
	descriptorNormalMapWrite.dstBinding = 2;
	descriptorNormalMapWrite.dstArrayElement = 0;
	descriptorNormalMapWrite.descriptorCount = 1;
	descriptorNormalMapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorNormalMapWrite.pImageInfo = &descriptorNormalMapInfo;
	descriptorNormalMapWrite.pBufferInfo = nullptr;
	descriptorNormalMapWrite.pTexelBufferView = nullptr;
	descriptorSetWrites.push_back(descriptorNormalMapWrite);

	//Sampler info (for texcoords normalMap in fragment shader).
	VkDescriptorImageInfo descriptorEmissionMapInfo;
	descriptorEmissionMapInfo.sampler = mesh->GetTextures()[2]->GetSampler();
	descriptorEmissionMapInfo.imageView = mesh->GetTextures()[2]->GetImageView();
	descriptorEmissionMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Sampler write using descriptorSamplerInfo.
	VkWriteDescriptorSet descriptorEmissionMapWrite;
	descriptorEmissionMapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorEmissionMapWrite.pNext = nullptr;
	descriptorEmissionMapWrite.dstSet = this->descriptorSets[setIndex];
	descriptorEmissionMapWrite.dstBinding = 3;
	descriptorEmissionMapWrite.dstArrayElement = 0;
	descriptorEmissionMapWrite.descriptorCount = 1;
	descriptorEmissionMapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorEmissionMapWrite.pImageInfo = &descriptorEmissionMapInfo;
	descriptorEmissionMapWrite.pBufferInfo = nullptr;
	descriptorEmissionMapWrite.pTexelBufferView = nullptr;
	descriptorSetWrites.push_back(descriptorEmissionMapWrite);

	//Sampler info (for texcoords normalMap in fragment shader).
	VkDescriptorImageInfo descriptorRoughnessMapInfo;
	descriptorRoughnessMapInfo.sampler = mesh->GetTextures()[3]->GetSampler();
	descriptorRoughnessMapInfo.imageView = mesh->GetTextures()[3]->GetImageView();
	descriptorRoughnessMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Sampler write using descriptorSamplerInfo.
	VkWriteDescriptorSet descriptorRoughnessMapWrite;
	descriptorRoughnessMapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorRoughnessMapWrite.pNext = nullptr;
	descriptorRoughnessMapWrite.dstSet = this->descriptorSets[setIndex];
	descriptorRoughnessMapWrite.dstBinding = 4;
	descriptorRoughnessMapWrite.dstArrayElement = 0;
	descriptorRoughnessMapWrite.descriptorCount = 1;
	descriptorRoughnessMapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorRoughnessMapWrite.pImageInfo = &descriptorRoughnessMapInfo;
	descriptorRoughnessMapWrite.pBufferInfo = nullptr;
	descriptorRoughnessMapWrite.pTexelBufferView = nullptr;
	descriptorSetWrites.push_back(descriptorRoughnessMapWrite);

	//Sampler info (for texcoords normalMap in fragment shader).
	VkDescriptorImageInfo descriptorAmbientMapInfo;
	descriptorAmbientMapInfo.sampler = mesh->GetTextures()[4]->GetSampler();
	descriptorAmbientMapInfo.imageView = mesh->GetTextures()[4]->GetImageView();
	descriptorAmbientMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Sampler write using descriptorSamplerInfo.
	VkWriteDescriptorSet descriptorAmbientMapWrite;
	descriptorAmbientMapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorAmbientMapWrite.pNext = nullptr;
	descriptorAmbientMapWrite.dstSet = this->descriptorSets[setIndex];
	descriptorAmbientMapWrite.dstBinding = 5;
	descriptorAmbientMapWrite.dstArrayElement = 0;
	descriptorAmbientMapWrite.descriptorCount = 1;
	descriptorAmbientMapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorAmbientMapWrite.pImageInfo = &descriptorAmbientMapInfo;
	descriptorAmbientMapWrite.pBufferInfo = nullptr;
	descriptorAmbientMapWrite.pTexelBufferView = nullptr;
	descriptorSetWrites.push_back(descriptorAmbientMapWrite);

	//Update descriptorset using writes size and data.
	vkUpdateDescriptorSets(this->logicalDevice, static_cast<ui32>(descriptorSetWrites.size()), descriptorSetWrites.data(), 0, nullptr);
}

//Create all buffers for one object. A lot of references needed for this
void Rendering::CreateObjectBuffers(VkBuffer& vertexBuffer, VkBuffer& indexBuffer, VkBuffer& uniformBuffer, VkDeviceMemory& vertexBufferMemory, VkDeviceMemory& indexBufferMemory, VkDeviceMemory& uniformBufferMemory, const char* meshName, bool instanced)
{
	VkDeviceSize bufferSize;
	bufferSize = sizeof(VertexInputInfo);

	this->CreateBufferOnGPU(this->GetVerticesOfObject(meshName), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer, vertexBufferMemory);
	this->CreateBufferOnGPU(this->GetIndicesOfObject(meshName), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer, indexBufferMemory);
	this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, uniformBuffer, (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), uniformBufferMemory);
}
//Create all buffers for all objects
void Rendering::CreateBuffersForObjects(void)
{
	//Get scene
	Scene* scene = SceneManager::GetInstancePtr()->GetActiveScene();

	//For all gameobjects
	for (Gameobject* gb : scene->GetAllGameobjects())
	{
		//If gb is renderable
		if (gb->GetIsRenderable())
		{
			//If not initalized
			if (scene->GetMesh(gb->GetName())->GetInitStatus())
				continue;

			//Set initalized to true
			scene->GetMesh(gb->GetName())->GetInitStatus() = true;

			//If buffer not created yet
			if (!gb->GetBufferCreated())
			{
				//Created buffer true
				gb->GetBufferCreated() = true;

				//Set Indices size
				scene->GetMesh(gb->GetName())->SetIndicesSize(
					static_cast<ui32>(
						this->GetIndicesOfObject(scene->GetMesh(gb->GetName())->GetName()).size()
						)
				);

				//Load and upload all textures of mesh
				for (Texture* tex : scene->GetMesh(gb->GetName())->GetTextures())
				{
					tex->Load();
					tex->Upload();
				}

				//Create all buffers
				CreateObjectBuffers
				(
					scene->GetMesh(gb->GetName())->GetVertexBuffer(),
					scene->GetMesh(gb->GetName())->GetIndexBuffer(),
					scene->GetMesh(gb->GetName())->GetUniformBuffer(),
					scene->GetMesh(gb->GetName())->GetVertexBufferMem(),
					scene->GetMesh(gb->GetName())->GetIndexBufferMem(),
					scene->GetMesh(gb->GetName())->GetUniformBufferMem(),
					scene->GetMesh(gb->GetName())->GetName().c_str(), false
				);
			}
		}
	}
}

//Create pipeline using all the infos and stages defined in the pipeline info.
void Rendering::CreatePipeline()
{
	//Create shader modules.
	this->CreateShaderModules();

	//SHADERSTAGE INFOS
	//Vertex shader stage info.
	VkPipelineShaderStageCreateInfo vertShaderStageInfo;
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.pNext = nullptr;
	vertShaderStageInfo.flags = 0;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = this->vertexModule;
	vertShaderStageInfo.pName = "main";
	vertShaderStageInfo.pSpecializationInfo = nullptr; //Constant variables

	//Fragment shader stage info.
	VkPipelineShaderStageCreateInfo fragShaderStageInfo;
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.pNext = nullptr;
	fragShaderStageInfo.flags = 0;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = this->fragmentModule;
	fragShaderStageInfo.pName = "main";
	fragShaderStageInfo.pSpecializationInfo = nullptr; //Constant variables

	//Store shader stage infos.
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//Get bindings and attributes of a vertex point.
	std::vector<VkVertexInputBindingDescription> bindings;

	VkVertexInputBindingDescription binding = Vertex::GetBindingDescription();
	bindings.push_back(binding);
	std::vector<VkVertexInputAttributeDescription> attributes = Vertex::GetAttributeDescriptions();

	//VERTEX INPUT
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pNext = nullptr;
	vertexInputInfo.flags = 0;
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<ui32>(bindings.size());
	vertexInputInfo.pVertexBindingDescriptions = bindings.data(); 
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<ui32>(attributes.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributes.data();

	//INPUT ASSEMBLY
	VkPipelineInputAssemblyStateCreateInfo inputAsseblyInfo;
	inputAsseblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAsseblyInfo.pNext = nullptr;
	inputAsseblyInfo.flags = 0;
	inputAsseblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAsseblyInfo.primitiveRestartEnable = VK_FALSE;

	//VIEWPORT
	VkViewport pipelineViewport;
	pipelineViewport.x = 0.0f;
	pipelineViewport.y = 0.0f;
	pipelineViewport.width = static_cast<float>(this->surfaceCapabilities.currentExtent.width);
	pipelineViewport.height = static_cast<float>(this->surfaceCapabilities.currentExtent.height);
	pipelineViewport.minDepth = 0.0f;
	pipelineViewport.maxDepth = 1.0f;

	//Scissor saying what part of the window should be drawn.
	VkRect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent = { this->surfaceCapabilities.currentExtent.width, this->surfaceCapabilities.currentExtent.height };

	this->screenScissor = scissor;
	this->viewport = pipelineViewport;

	//Viewport info using viewport and scissors.
	VkPipelineViewportStateCreateInfo viewportInfo;
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.pNext = nullptr;
	viewportInfo.flags = 0;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &pipelineViewport;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &scissor;

	//RASTERIZATION
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationInfo.pNext = nullptr;
	rasterizationInfo.flags = 0;
	rasterizationInfo.depthClampEnable = VK_FALSE;
	rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationInfo.depthBiasEnable = VK_FALSE; //Add constant value onto fragment depth
	rasterizationInfo.depthBiasConstantFactor = 0.0f;
	rasterizationInfo.depthBiasClamp = 0.0f;
	rasterizationInfo.depthBiasSlopeFactor = 0.0f;
	rasterizationInfo.lineWidth = 1.0f;

	//MULTISAMPLING
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleInfo.pNext = nullptr;
	multisampleInfo.flags = 0;
	multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleInfo.sampleShadingEnable = VK_FALSE;
	multisampleInfo.minSampleShading = 1.0f;
	multisampleInfo.pSampleMask = nullptr;
	multisampleInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleInfo.alphaToOneEnable = VK_FALSE;

	//BLENDMODE (Alpha blending: current.alpha * current.rgb + (1 - current.alpha) * previous.rgb)
	VkPipelineColorBlendAttachmentState colorBlendState;
	colorBlendState.blendEnable = VK_TRUE;
	colorBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	//Blend state info using color blend state attachment. 
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendInfo.pNext = nullptr;
	colorBlendInfo.flags = 0;
	colorBlendInfo.logicOpEnable = VK_FALSE;
	colorBlendInfo.logicOp = VK_LOGIC_OP_CLEAR;
	colorBlendInfo.attachmentCount = 1;
	colorBlendInfo.pAttachments = &colorBlendState;
	colorBlendInfo.blendConstants[0] = 0.0f;
	colorBlendInfo.blendConstants[1] = 0.0f;
	colorBlendInfo.blendConstants[2] = 0.0f;
	colorBlendInfo.blendConstants[3] = 0.0f;

	//Define dynamic states, that change through recreating the swapchain.
	VkDynamicState dynamicStates[]
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	//Dynamic state info using the dynamic states (used so viewport and scissors can be changed at runtime without recreating the whole pipeline).
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.pNext = nullptr;
	dynamicStateInfo.flags = 0;
	dynamicStateInfo.dynamicStateCount = 2;
	dynamicStateInfo.pDynamicStates = dynamicStates;

	//PIPELINE LAYOUT
	VkPipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = nullptr;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &this->descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; //TODO
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	//Create pipeline layout using the pipeline info.
	VK_CHECK(vkCreatePipelineLayout(this->logicalDevice, &pipelineLayoutInfo, nullptr, &this->pipelineLayout));

	//CREATE PIPELINE
	VkGraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = static_cast<ui32>(sizeof(shaderStages) / sizeof(VkPipelineShaderStageCreateInfo));
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAsseblyInfo;
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pViewportState = &viewportInfo;
	pipelineInfo.pRasterizationState = &rasterizationInfo;
	pipelineInfo.pMultisampleState = &multisampleInfo;
	pipelineInfo.pDepthStencilState = &this->depthImage.GetDepthInfo();
	pipelineInfo.pColorBlendState = &colorBlendInfo;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.layout = this->pipelineLayout;
	pipelineInfo.renderPass = this->renderpass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -10; //invalid index for creation.

	//Create pipeline using all the infos and stages defined in the pipeline info.
	VK_CHECK(vkCreateGraphicsPipelines(this->logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->pipeline));
}
//Create renderpass using the declared dependencies and descriptions for the different representations.
void Rendering::CreateRenderpass()
{
	//ATTACHMENTS
	//Attachment description for normal color representation.
	VkAttachmentDescription attachmentDescription;
	attachmentDescription.flags = 0;
	attachmentDescription.format = this->supportedColorFormat;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//Attachment reference for normal color representation.
	VkAttachmentReference attachmentReference;
	attachmentReference.attachment = 0;
	attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//Attachment description for depth representation.
	VkAttachmentDescription depthDescription = this->depthImage.GetAttachmentDescription(this->physicalDevice);

	//Attachment reference for depth representation.
	VkAttachmentReference depthAttachmentReference;
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//SUBPASSES
	VkSubpassDescription subpassDescription;
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &attachmentReference;
	subpassDescription.pResolveAttachments = nullptr; //multisampling
	subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr; //Keep those attachments

	//Declare dependencies of subpass.
	VkSubpassDependency subpassDependency;
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependency.dependencyFlags = 0;

	//Store the attachment descriptions.
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	attachmentDescriptions.push_back(attachmentDescription);
	attachmentDescriptions.push_back(depthDescription);

	//RENDERPASS
	VkRenderPassCreateInfo renderpassInfo;
	renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpassInfo.pNext = nullptr;
	renderpassInfo.flags = 0;
	renderpassInfo.attachmentCount = static_cast<ui32>(attachmentDescriptions.size());
	renderpassInfo.pAttachments = attachmentDescriptions.data();
	renderpassInfo.subpassCount = 1;
	renderpassInfo.pSubpasses = &subpassDescription;
	renderpassInfo.dependencyCount = 0;
	renderpassInfo.pDependencies = &subpassDependency;

	//Create renderpass using the declared dependencies and descriptions for the different representations.
	VK_CHECK(vkCreateRenderPass(this->logicalDevice, &renderpassInfo, nullptr, &this->renderpass));
}

//Create Frame buffers using the frame buffer infos.
void Rendering::CreateFramebuffers()
{
	//Create the same amount of framebuffers as the swapchain images.
	this->framebuffers.resize(this->swapchainImages.size());
	for (size_t i = 0; i < this->swapchainImages.size(); i++)
	{
		//Store attachment views for the normal image and the depth image.
		std::vector<VkImageView> attachmentViews;
		attachmentViews.push_back(this->imageViews[i]);
		attachmentViews.push_back(this->depthImage.GetDepthImageView());

		//Frame buffer info using the attachment views and surface information.
		VkFramebufferCreateInfo framebufferInfo;
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.flags = 0;
		framebufferInfo.renderPass = this->renderpass;
		framebufferInfo.attachmentCount = static_cast<ui32>(attachmentViews.size());
		framebufferInfo.pAttachments = attachmentViews.data();
		framebufferInfo.width = this->surfaceCapabilities.currentExtent.width;
		framebufferInfo.height = this->surfaceCapabilities.currentExtent.height;
		framebufferInfo.layers = 1;

		//Create Frame buffer using the frame buffer info.
		VK_CHECK(vkCreateFramebuffer(this->logicalDevice, &framebufferInfo, nullptr, &(this->framebuffers[i])));
	}
}

//Create Command pool
void Rendering::CreateCommandPool(void)
{
	//Command pool info.
	VkCommandPoolCreateInfo commandPoolInfo;
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;
	commandPoolInfo.flags = 0; 
	commandPoolInfo.queueFamilyIndex = this->indexOfGraphicsQueue;

	//Create command pool and setting it up to run on the graphics queue.
	VK_CHECK(vkCreateCommandPool(this->logicalDevice, &commandPoolInfo, nullptr, &this->m_commandPool));
}
//Allocate command buffer.
void Rendering::CreateCommandbuffer()
{
	//Allocate command buffer using the command pool and swapchain images amount.
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = this->m_commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = static_cast<ui32>(this->swapchainImages.size());

	this->commandBuffers.resize(this->swapchainImages.size());
	vkAllocateCommandBuffers(this->logicalDevice, &commandBufferAllocateInfo, this->commandBuffers.data());
}

//Create depth image.
void Rendering::CreateDepthImage(void)
{
	//Create depth image using the command pool. physical device, graphics queue and surface capabilities.
	this->depthImage.Create(this->physicalDevice, this->m_commandPool, this->graphicsQueue, this->surfaceCapabilities.currentExtent.width, this->surfaceCapabilities.currentExtent.height);
}
//Create semaphores.
void Rendering::CreateSemaphores()
{
	//Create semaphores (coordinate graphics queue and present queue) for if the image is available and if the image was rendered. 
	VkSemaphoreCreateInfo semaphoreInfo;
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = nullptr;
	semaphoreInfo.flags = 0;

	VK_CHECK(vkCreateSemaphore(this->logicalDevice, &semaphoreInfo, nullptr, &this->imageAvailableSemaphore));
	VK_CHECK(vkCreateSemaphore(this->logicalDevice, &semaphoreInfo, nullptr, &this->renderingFinishedSemaphore));

	this->waitStageMask.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
}

//Record commands for the graphics queue.
void Rendering::RecordCommands()
{
	//Command buffer begin info.
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	//Get all renderable gbs
	std::vector<std::string> tempGbs;
	for (Gameobject* gb : SceneManager::GetInstancePtr()->GetActiveScene()->GetAllGameobjects())
	{
		if (gb->GetIsRenderable() && gb->GetIsActive())
			tempGbs.push_back(gb->GetName());
	}

	//For every image in swapchain
	for (size_t i = 0; i < this->swapchainImages.size(); i++)
	{
		//Start recording.
		vkBeginCommandBuffer(this->commandBuffers[i], &commandBufferBeginInfo);

		//Declare and store clear values of normal color image and depth image.
		std::vector <VkClearValue> clearValues;

		VkClearValue clearValue = { };
		clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };

		VkClearValue depthClearValue = { 1.0f, 0 };

		clearValues.push_back(clearValue);
		clearValues.push_back(depthClearValue);

		//Renderpass begin info using the renderpass, framebuffer, surface extent and clear values.
		VkRenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = this->renderpass;
		renderPassBeginInfo.framebuffer = this->framebuffers[i];
		renderPassBeginInfo.renderArea.extent = this->surfaceCapabilities.currentExtent;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.clearValueCount = static_cast<ui32>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		//Begin renderpass.
		vkCmdBeginRenderPass(this->commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		//Set viewport and scissors.
		this->viewport.width = static_cast<float>(this->surfaceCapabilities.currentExtent.width);
		this->viewport.height = static_cast<float>(this->surfaceCapabilities.currentExtent.height);
		this->screenScissor.extent = { this->surfaceCapabilities.currentExtent.width, this->surfaceCapabilities.currentExtent.height };

		vkCmdSetViewport(this->commandBuffers[i], 0, 1, &this->viewport);
		vkCmdSetScissor(this->commandBuffers[i], 0, 1, &this->screenScissor);

		//Define offset for drawing the mesh instances.
		VkDeviceSize offsets[] = { 0 };

		Scene* scene = SceneManager::GetInstancePtr()->GetActiveScene();

		//Bind vertex, index buffers, descriptor set
		//Draw model
		if (tempGbs.size() > 0)
		{
			vkCmdBindPipeline(this->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);

			for (size_t j = 0; j < tempGbs.size(); j++)
			{
				vkCmdBindVertexBuffers(this->commandBuffers[i], 0, 1, &scene->GetMesh(tempGbs[j])->GetVertexBuffer(), offsets);
				vkCmdBindIndexBuffer(this->commandBuffers[i], scene->GetMesh(tempGbs[j])->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
				vkCmdBindDescriptorSets(this->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayout, 0, 1, &this->descriptorSets[j], 0, nullptr);
				vkCmdDrawIndexed(this->commandBuffers[i], scene->GetMesh(tempGbs[j])->GetIndicesSize(), 1, 0, 0, 0);
			}
		}

		//End renderpass.
		vkCmdEndRenderPass(this->commandBuffers[i]);

		//End recording of command buffer.
		vkEndCommandBuffer(this->commandBuffers[i]);
	}
}
//Recreate buffers, descriptor sets, and commands TODO
void Rendering::ReRecordCommands(void)
{
	vkDeviceWaitIdle(this->logicalDevice);
	 
	if (this->descriptorSets.size() > 0)
	{
		vkFreeDescriptorSets(this->logicalDevice, this->descriptorPool, static_cast<ui32>(this->descriptorSets.size()), this->descriptorSets.data());
		this->descriptorSets.clear();
	}
	vkDestroyDescriptorPool(this->logicalDevice, this->descriptorPool, nullptr);

	vkFreeCommandBuffers(this->logicalDevice, this->m_commandPool, static_cast<ui32>(this->commandBuffers.size()), this->commandBuffers.data());
	
	this->CreateDescriptorPool();
	this->CreateBuffersForObjects();
	this->CreateDescriptorSets();
	this->CreateCommandbuffer();
	this->RecordCommands();
}

//Update vertex shader with MVP and vertex data
void Rendering::UpdateMVP()
{
	Scene* scene = SceneManager::GetInstancePtr()->GetActiveScene();

	//Update MVP Matrix for every object in the scene that isnt instanced
	for (Gameobject* gb : scene->GetAllGameobjects())
	{
		if (gb->GetIsRenderable() && gb->GetIsActive())
		{
			//Set vertex data
			VertexInputInfo vertexInfo;
			vertexInfo.modelMatrix = gb->GetModelMatrix();
			vertexInfo.lightPosition = SceneManager::GetInstancePtr()->GetActiveCamera()->GetPostion();
			vertexInfo.viewMatrix = SceneManager::GetInstancePtr()->GetActiveCamera()->GetViewMatrix();
			vertexInfo.projectionMatrix = SceneManager::GetInstancePtr()->GetActiveCamera()->GetProjectionMatrix();
			vertexInfo.color = scene->GetMaterial(gb->GetName())->fragColor;
			vertexInfo.ambientVal = 0.1f;
			vertexInfo.specularVal = 16.0f;

			//Map vertex shader, copy information to it and unmap it again
			void* data;
			vkMapMemory(this->logicalDevice, scene->GetMesh(gb->GetName())->GetUniformBufferMem(), 0, sizeof(vertexInfo), 0, &data);
			memcpy(data, &vertexInfo, sizeof(vertexInfo));
			vkUnmapMemory(this->logicalDevice, scene->GetMesh(gb->GetName())->GetUniformBufferMem());
		}
	}

	scene = nullptr;
}
//Draw frame
void Rendering::DrawFrame()
{
	//Get image index in swapchain.
	ui32 imageIndex;
	vkAcquireNextImageKHR(this->logicalDevice, this->swapchain, INT_MAX, this->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex); 

	//Create submit info for graphics queue.
	VkSubmitInfo graphicsInfo;
	graphicsInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	graphicsInfo.pNext = nullptr;
	graphicsInfo.waitSemaphoreCount = 1;
	graphicsInfo.pWaitSemaphores = &this->imageAvailableSemaphore;
	graphicsInfo.pWaitDstStageMask = this->waitStageMask.data();
	graphicsInfo.commandBufferCount = 1;
	graphicsInfo.pCommandBuffers = &(this->commandBuffers[imageIndex]);
	graphicsInfo.signalSemaphoreCount = 1;
	graphicsInfo.pSignalSemaphores = &this->renderingFinishedSemaphore;

	//Submit graphics queue.
	vkQueueSubmit(this->graphicsQueue, 1, &graphicsInfo, VK_NULL_HANDLE);

	//Create submit info for present queue.
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &this->renderingFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &this->swapchain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	//If present is available present via present queue. If not present via graphics queue.
	if (this->presentQueue != VK_NULL_HANDLE)
		vkQueuePresentKHR(this->presentQueue, &presentInfo);
	else if (this->canGraphicsQueuePresent)
		vkQueuePresentKHR(this->graphicsQueue, &presentInfo);
	else
		throw;

}

//Rate physical devices on different factors and check for queues that are supported. Pick the best physical device.
void Rendering::PickIdealPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices)
{
	std::vector<ui32> ratings;

	for (const VkPhysicalDevice& phyDevice : physicalDevices)
	{
		ui32 rating = 0;


		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(phyDevice, &properties);

		//If GPU is not on mainboard give 1000 points if it is give less.
		switch (properties.deviceType)
		{
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				rating += 1000;
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				rating += 600;
				break;
			default:
				rating += 10;
				break;
		}

		VkPhysicalDeviceFeatures deviceFeatures = { };
		vkGetPhysicalDeviceFeatures(phyDevice, &deviceFeatures);

		if (deviceFeatures.geometryShader)
			rating += 100;

		ui32 queueFamiliyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamiliyCount, nullptr);

		std::vector<VkQueueFamilyProperties> familiyProperties(queueFamiliyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamiliyCount, familiyProperties.data());

		//For every supported queue family check different features and give ratings.
		for (int i = 0; i < familiyProperties.size(); i++)
		{
			//Can queue present an image on the surface?
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(phyDevice, i, this->surface, &presentSupport);

			//If queueFamily is supporting graphics operations. Most important.
			if ((familiyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && this->indexOfGraphicsQueue == INT_MAX)
			{
				rating += 400;

				//override if already taken.
				if (ratings.size() == 0)
				{
					this->indexOfGraphicsQueue = i;
					this->queueCount++;
				}
				else if (ratings.size() != 0 && (rating + 400) >= ratings[i])
				{
					this->indexOfGraphicsQueue = i;
				}

				if (presentSupport)
					this->canGraphicsQueuePresent = true;
				continue;
			}
			//If this queueFamily is supporting transfer operations (e.g. transfering buffer from CPU to GPU) without supporting graphics operations.
			else if ((familiyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0 && (familiyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 && this->indexOfTransferQueue == INT_MAX)
			{
				rating += 200;

				//override if already taken.
				if (ratings.size() == 0)
				{
					this->indexOfTransferQueue = i;
					this->queueCount++;
				}
				else if (ratings.size() != 0 && (rating + 200) >= ratings[i])
				{
					this->indexOfTransferQueue = i;
				}
				continue;
			}
			//If this queueFamily is only supporting presentation of an image on a surface.
			else if (familiyProperties[i].queueCount > 0 && presentSupport)
			{
				rating += 200;

				//override if already taken.
				if (ratings.size() == 0)
				{
					this->indexOfPresentQueue = i;
					this->queueCount++;
				}
				else if (ratings.size() != 0 && (rating) >= ratings[i])
				{
					this->indexOfPresentQueue = i;
				}
				continue;
			}
			//Other family properties.
			else if ((familiyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
				rating += 5;
			else if ((familiyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)
				rating += 10;
			else if ((familiyProperties[i].queueFlags & VK_QUEUE_PROTECTED_BIT) != 0)
				rating += 10;

			rating += familiyProperties[i].queueCount;
		}

		ratings.push_back(rating);
	}

	ui32 highestValue = 0;

	//Set this->physicalDevice to the physicalDevice with the highest rating.
	for (ui16 i = 0; i < ratings.size(); i++)
	{
		if (ratings[i] > highestValue)
		{
			this->physicalDevice = physicalDevices[i];
			highestValue = ratings[i];
		}
	}

	//Set Transfer and Present queue to graphics queue if the unique version doesnt exist
	//because graphics queue always supports transfering and presenting operations.
	if (this->indexOfTransferQueue == INT_MAX)
		this->indexOfTransferQueue = this->indexOfGraphicsQueue;
	if (this->indexOfPresentQueue == INT_MAX)
		this->indexOfPresentQueue = this->indexOfGraphicsQueue;

}

//Copy and transfer buffer to GPU side.
void Rendering::CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
	//Allocate buffer info using the command pool.
	VkCommandBufferAllocateInfo commandBufferInfo;
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.commandPool = this->m_commandPool;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount = 1;

	//Create command pool and use it for transfering buffer if the transfer queue is available.
	VkCommandPool transferCommandPool = VK_NULL_HANDLE;
	if (this->indexOfTransferQueue != this->indexOfGraphicsQueue)
	{
		VkCommandPoolCreateInfo commandPoolInfo;
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.pNext = nullptr;
		commandPoolInfo.flags = 0; 
		commandPoolInfo.queueFamilyIndex = this->indexOfTransferQueue;

		vkCreateCommandPool(this->logicalDevice, &commandPoolInfo, nullptr, &transferCommandPool);

		commandBufferInfo.commandPool = transferCommandPool;
	}

	//Allocate command buffer.
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(this->logicalDevice, &commandBufferInfo, &commandBuffer);

	//Begin recording of the command buffer.
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

	//Set buffer copy infos.
	VkBufferCopy bufferCopy;
	bufferCopy.srcOffset = 0;
	bufferCopy.dstOffset = 0;
	bufferCopy.size = size;

	//Copy the buffer.
	vkCmdCopyBuffer(commandBuffer, src, dst, 1, &bufferCopy);

	//End recording of the command buffer.
	vkEndCommandBuffer(commandBuffer);

	//Submit info using the command buffer.
	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	//Submit to queue. If transfer queue is available then take it. If not take graphics queue.
	if (this->transferQueue != VK_NULL_HANDLE)
	{
		vkQueueSubmit(this->transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(this->transferQueue);
	}
	else
	{
		vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE); //Transfer bit is included with graphics bit.
		vkQueueWaitIdle(this->graphicsQueue);
	}

	//Free command buffers and if transfer queue is available also delete the newly created transfer command pool.
	if (this->transferQueue != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(this->logicalDevice, transferCommandPool, 1, &commandBuffer);
		vkDestroyCommandPool(this->logicalDevice, transferCommandPool, nullptr);
	}
	else
	{
		vkFreeCommandBuffers(this->logicalDevice, this->m_commandPool, 1, &commandBuffer);
	}
}

//Load models.
void Rendering::LoadModels(void)
{
	//Get all model paths
	std::vector<std::string> paths = Application::GetInstancePtr()->GetFilesystem()->FilesInDirectory("models");

	this->models.resize(paths.size());

	//For all model files create a model
	for(size_t i = 0; i < paths.size(); i++)
	{
		this->models[i] = new Model();
		this->models[i]->Create(paths[i].c_str());
	}

	//Get scene
	Scene* scene = SceneManager::GetInstancePtr()->GetActiveScene();

	for (Gameobject* gb : scene->GetAllGameobjects())
	{
		for (ui32 i = 0; i < this->models.size(); i++)
		{
			std::string stringName(scene->GetMesh(gb->GetName())->GetName());
			if (this->models[i]->GetName() == stringName)
			{
				//Set model ID
				gb->GetModelID() = i;

				Math::Vec3 max = Math::Vec3::zero;
				Math::Vec3 min = Math::Vec3::zero;

				//Get min and max vertex of the model
				for (Vertex vertex : this->models[i]->GetVertices())
				{
					if (vertex.position.x > max.x)
					{
						max.x = vertex.position.x;
					}
					if (vertex.position.y > max.y)
					{
						max.y = vertex.position.y;
					}
					if (vertex.position.z > max.z)
					{
						max.z = vertex.position.z;
					}

					if (vertex.position.x < min.x)
					{
						min.x = vertex.position.x;
					}
					if (vertex.position.y < min.y)
					{
						min.y = vertex.position.y;
					}
					if (vertex.position.z < min.z)
					{
						min.z = vertex.position.z;
					}
				}

				//Set radius
				float radius = Math::Distance(Math::Vec3{ min.x - 8.0f, max.y + 1.0f, 0.0f },
					Math::Vec3{ min.x + 8.0f, max.y + 1.0f, 0.0f });
				gb->GetRadius() = radius;
			}
		}
	}
}


//Get Buffer in bytes (unsigned char).
std::vector<byte> Rendering::GetBuffer(RenderingBuffer bufferType)
{
	std::vector<byte> buffer;

	//Read data from file for the inputed buffer type.
	switch (bufferType)
	{
		case RenderingBuffer::VERTEX:
		{
			WinFile* vertexShaderFile = new WinFile(Application::GetInstancePtr()->GetFilesystem()->FileInDirectory("shader", "shader.vert").c_str());
			buffer.assign(vertexShaderFile->Read(), vertexShaderFile->Read() + vertexShaderFile->GetSize()); //trick to copy data to vector using pointer arithmetic.
			vertexShaderFile->Cleanup();
			break;
		}
		case RenderingBuffer::FRAGMENT:
		{
			WinFile* fragmentShaderFile = new WinFile(Application::GetInstancePtr()->GetFilesystem()->FileInDirectory("shader", "shader.frag").c_str());
			buffer.assign(fragmentShaderFile->Read(), fragmentShaderFile->Read() + fragmentShaderFile->GetSize()); //trick to copy data to vector using pointer arithmetic.
			fragmentShaderFile->Cleanup();
			break;
		}
	}

	return buffer;
}

//Get first supported format that fills the inputed requirements.
VkFormat Rendering::GetSupportedFormats(VkPhysicalDevice phyDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags)
{
	for (VkFormat format : formats)
	{
		if (this->isFormatSupported(phyDevice, format, tiling, featureFlags))
			return format;
	}

	return formats[0];
}

//Get the optimal supported surface color space.
VkColorSpaceKHR Rendering::GetSupportedSurfaceColorSpace(const std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats)
{
	for (ui16 i = 0; i < supportedSurfaceFormats.size(); i++)
	{
		if (supportedSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}

	return supportedSurfaceFormats[0].colorSpace;
}

//Check if a format is supported.
bool Rendering::isFormatSupported(VkPhysicalDevice phyDevice, VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags featureFlags)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(phyDevice, format, &formatProperties);

	if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & featureFlags) == featureFlags)
		return true;
	else if (tiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & featureFlags) == featureFlags)
		return true;

	return false;
}
//Check if a present mode is supported.
bool Rendering::isModeSupported(const std::vector<VkPresentModeKHR>& supportedPresentModes, VkPresentModeKHR presentMode)
{
	for (const VkPresentModeKHR& supportedPresentMode : supportedPresentModes)
	{
		if (supportedPresentMode == presentMode)
			return true;
	}

	return false;
}