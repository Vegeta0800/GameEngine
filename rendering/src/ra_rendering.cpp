
#define VK_USE_PLATFORM_WIN32_KHR
// EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
#include <list>

// INTERNAL INCLUDES
#include "ra_gameobject.h"
#include "ra_mesh.h"
#include "ra_rendering.h"
#include "ra_vkcheck.h"
#include "ra_types.h"
#include "ra_utils.h"
#include "ra_window.h"
#include "ra_application.h"
#include "filesystem/ra_filesystem.h"
#include "filesystem/ra_winfile.h"
#include "math/ra_mat4x4.h"
#include "input/ra_inputhandler.h"
#include "ra_depthimage.h"

DECLARE_SINGLETON(Rendering)

void Rendering::Initialize(const char* applicationName, ui32 applicationVersion)
{
	this->root = new Gameobject;
	this->root->Initialize();
	this->root->MakeRoot();

	this->testObject = new Gameobject;
	this->testObject->Initialize();
	this->testObject->SetParent(this->root);

	this->cameraPos = Math::Vec3{ 0.36f, 4.0f, 0.72f };

	this->testObject->GetTransform().position = Math::Vec3{0.0f, 0.0f, 0.0f};
	this->testObject->GetTransform().scaling *= 0.1f;

	for (Gameobject* gb : this->root->GetAllChildren())
	{
		if (!gb->hasRoot())
		{
			this->gameObjects.push_back(gb);
		}
	}

	this->vertexBuffers.resize(this->gameObjects.size());
	this->indexBuffers.resize(this->gameObjects.size());

	this->vertexBufferMemories.resize(this->gameObjects.size());
	this->indexBufferMemories.resize(this->gameObjects.size());

	this->CreateInstance(applicationName, applicationVersion);
	this->CreatePhysicalDevice();
	this->CreateSurface();
	this->SetupQueues();
	this->CreateLogicalDevice();
	this->CreateSwapChain();
	this->CreateImageViews();
	this->CreateRenderpass();
	this->CreateDescriptorSetLayout();
	this->CreatePipeline();
	this->CreateCommandPool();
	this->CreateDepthImage();
	this->CreateFramebuffers();
	this->CreateCommandbuffer();
	this->LoadTexture();
	this->LoadModels();
	this->CreateVertexbuffers();
	this->CreateIndexbuffers();
	this->CreateUniformbuffer();
	this->CreateDescriptorPool();
	this->CreateDescriptorSet();
	this->RecordCommands();
	this->CreateSemaphores();

	this->initialized = true;
}

void Rendering::Update(float time)
{
	this->UpdateMVP(time);
	this->DrawFrame();
}

void Rendering::UpdateMVP(float time)
{
	if (Input::GetInstancePtr()->GetKey(KeyCode::D))
	{
		this->cameraPos.x -= 0.0002f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::A))
	{
		this->cameraPos.x += 0.0002f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::W))
	{
		this->cameraPos.z -= 0.0002f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::S))
	{
		this->cameraPos.z += 0.0002f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::Q))
	{
		this->cameraPos.y -= 0.0002f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::E))
	{
		this->cameraPos.y += 0.0002f * time;
	}

	this->testObject->GetTransform().eulerRotation.z = time * 20.0f;


	printf("%f, %f, %f \n", this->cameraPos.x, this->cameraPos.y, this->cameraPos.z);

	Math::Mat4x4 modelMatrix = this->testObject->GetModelMatrix();
	Math::Mat4x4 viewMatrix = Math::CreateViewMatrixLookAt(this->cameraPos, Math::Vec3::zero, Math::Vec3::unit_z);
	Math::Mat4x4 projectionMatrix = Math::CreateProjectionMatrix(DegToRad(45.0f), (float)this->surfaceCapabilities.currentExtent.width / (float)this->surfaceCapabilities.currentExtent.height, 0.1f, 100.0f);
	projectionMatrix.m22 *= -1.0f;
	this->mvp = modelMatrix;
	this->mvp = this->mvp * viewMatrix;
	this->mvp = this->mvp * projectionMatrix;

	void* data;
	vkMapMemory(this->logicalDevice, this->uniformBufferMemory, 0, sizeof(this->mvp), 0, &data);
	memcpy(data, &this->mvp, sizeof(this->mvp));
	vkUnmapMemory(this->logicalDevice, this->uniformBufferMemory);
}

void Rendering::BeginRecording(VkCommandBuffer& commandBuffer, VkCommandPool commandPool, VkCommandBufferUsageFlags commandBufferUsageFlags)
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;

	VK_CHECK(vkAllocateCommandBuffers(this->logicalDevice, &commandBufferAllocateInfo, &commandBuffer));

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = commandBufferUsageFlags;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));
}

void Rendering::StopRecording(VkQueue queue, VkCommandBuffer commandBuffer, VkCommandPool commandPool)
{
	vkEndCommandBuffer(commandBuffer);

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

	VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(this->logicalDevice, commandPool, 1, &commandBuffer);
}

void Rendering::ChangeLayout(VkCommandPool commandPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	this->BeginRecording(commandBuffer, commandPool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkImageMemoryBarrier imageMemoryBarrier = { };
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else
		throw;

	imageMemoryBarrier.oldLayout = oldLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
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

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	this->StopRecording(queue, commandBuffer, commandPool);
}

void Rendering::DrawFrame()
{
	ui32 imageIndex;
	vkAcquireNextImageKHR(this->logicalDevice, this->swapchain, 1000000000, this->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex); //TODO Tweak timeoutLimit current 1 second.

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &this->imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = this->waitStageMask.data();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(this->commandBuffers[imageIndex]);
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &this->renderingFinishedSemaphore;

	vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &this->renderingFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &this->swapchain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	if (this->presentQueue != VK_NULL_HANDLE)
		vkQueuePresentKHR(this->presentQueue, &presentInfo);
	else if (this->canGraphicsQueuePresent)
		vkQueuePresentKHR(this->graphicsQueue, &presentInfo);
	else
		throw;
}

void Rendering::CreateInstance(const char* applicationName, ui32 applicationVersion)
{
	const std::vector<const char*> instanceValidationLayers =
	{
		"VK_LAYER_LUNARG_standard_validation",
	};

	const std::vector<const char*> instanceExtensions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	};

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = applicationName;
	appInfo.applicationVersion = applicationVersion;
	appInfo.pEngineName = "RA Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo instanceInfo = { };
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0; //TODO
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = static_cast<ui32>(instanceExtensions.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

#if defined(_DEBUG)
	instanceInfo.enabledLayerCount = static_cast<ui32>(instanceValidationLayers.size());
	instanceInfo.ppEnabledLayerNames = instanceValidationLayers.data();
#endif

	VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &this->vkInstance));
}

void Rendering::CreatePhysicalDevice()
{
	ui32 physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(this->vkInstance, &physicalDeviceCount, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(this->vkInstance, &physicalDeviceCount, physicalDevices.data()));

	PickIdealPhysicalDevice(physicalDevices);
}

void Rendering::PickIdealPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices)
{
	std::vector<ui32> ratings;

	for (const VkPhysicalDevice& phyDevice : physicalDevices)
	{
		ui32 rating = 0;

		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(phyDevice, &properties);

		switch (properties.deviceType)
		{
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				rating += 1000;
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				rating += 500;
				break;
			default:
				rating += 10;
				break;
		}

		VkPhysicalDeviceFeatures deviceFeatures = { };
		vkGetPhysicalDeviceFeatures(phyDevice, &deviceFeatures);

		if (deviceFeatures.geometryShader)
			rating += 200;

		ui32 queueFamiliyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamiliyCount, nullptr);

		std::vector<VkQueueFamilyProperties> familiyProperties(queueFamiliyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamiliyCount, familiyProperties.data());

		for (int i = 0; i < familiyProperties.size(); i++)
		{
			if ((familiyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				rating += 400;
			}
			if ((familiyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
				rating += 5;
			if ((familiyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
				rating += 10;
			if ((familiyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)
				rating += 10;
			if ((familiyProperties[i].queueFlags & VK_QUEUE_PROTECTED_BIT) != 0)
				rating += 10;

			rating += familiyProperties[i].queueCount;
		}

		ratings.push_back(rating);
	}

	ui32 highestValue = 0;

	for (int i = 0; i < ratings.size(); i++)
	{
		if (ratings[i] > highestValue)
		{
			this->physicalDevice = physicalDevices[i];
			highestValue = ratings[i];
		}
	}

}

void Rendering::SetupQueues(void)
{
	ui32 queueFamiliyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamiliyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamiliyProperties(queueFamiliyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamiliyCount, queueFamiliyProperties.data());

	for (int i = 0; i < queueFamiliyProperties.size(); i++)
	{
		if ((queueFamiliyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && this->indexOfGraphicsQueue == INT_MAX)
		{
			this->indexOfGraphicsQueue = i;
			this->queueCount++;
			this->queueFamilyIndices.push_back(i);

			VkBool32 presentSupportofGraphicsQueue = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(this->physicalDevice, i, this->surface, &presentSupportofGraphicsQueue);
			if (presentSupportofGraphicsQueue)
				this->canGraphicsQueuePresent = true;
			continue;
		}
		if ((queueFamiliyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0 && (queueFamiliyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 && this->indexOfTransferQueue == INT_MAX)
		{
			this->indexOfTransferQueue = i;
			this->queueFamilyIndices.push_back(i);
			this->queueCount++;
			continue;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(this->physicalDevice, i, this->surface, &presentSupport);

		if (queueFamiliyProperties[i].queueCount > 0 && presentSupport)
		{
			this->indexOfPresentQueue = i;
			this->queueFamilyIndices.push_back(i);
			this->queueCount++;
			continue;
		}
	}

	if (this->indexOfTransferQueue == INT_MAX)
		this->indexOfTransferQueue = this->indexOfGraphicsQueue;

	if (this->indexOfPresentQueue == INT_MAX)
		this->indexOfPresentQueue = this->indexOfGraphicsQueue;
}

void Rendering::CreateLogicalDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queueInfos;

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

	VkPhysicalDeviceFeatures enabledFeatures = { };
	enabledFeatures.samplerAnisotropy = VK_TRUE;


	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

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

	VK_CHECK(vkCreateDevice(this->physicalDevice, &deviceInfo, nullptr, &this->logicalDevice));

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

void Rendering::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo;
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0; //TODO
	surfaceInfo.hinstance = GetModuleHandle(NULL);
	surfaceInfo.hwnd = Window::GetInstancePtr()->GetHandle();

	VK_CHECK(vkCreateWin32SurfaceKHR(this->vkInstance, &surfaceInfo, nullptr, &this->surface));
}

void Rendering::CreateSwapChain(void)
{
	this->surfaceCapabilities;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDevice, this->surface, &this->surfaceCapabilities));

	ui32 formatsCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, this->surface, &formatsCount, nullptr);
	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, this->surface, &formatsCount, surfaceFormats.data()));

	ui32 presentationModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->physicalDevice, this->surface, &presentationModeCount, nullptr);
	std::vector<VkPresentModeKHR> presentModes(presentationModeCount);
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(this->physicalDevice, this->surface, &presentationModeCount, presentModes.data()));

	VkBool32 surfaceSupported = false;
	VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(this->physicalDevice, 0, this->surface, &surfaceSupported));

	if (!surfaceSupported)
	{
		printf("Surface not supported!");
		__debugbreak();
	}

	VkSwapchainCreateInfoKHR swapchainInfo;
	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.pNext = nullptr;
	swapchainInfo.flags = 0; //TODO
	swapchainInfo.surface = this->surface;
	swapchainInfo.minImageCount = this->surfaceCapabilities.maxImageCount >= 3 ? 3 : 2;
	swapchainInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainInfo.imageExtent.height = this->surfaceCapabilities.currentExtent.height;
	swapchainInfo.imageExtent.width = this->surfaceCapabilities.currentExtent.width;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //Change if more than one queue
	swapchainInfo.queueFamilyIndexCount = 0; //TODO if more queues
	swapchainInfo.pQueueFamilyIndices = nullptr; //TODO if more queuess
	swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.presentMode = this->isModeSupported(presentModes, VK_PRESENT_MODE_MAILBOX_KHR) ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = this->swapchain;

	VK_CHECK(vkCreateSwapchainKHR(this->logicalDevice, &swapchainInfo, nullptr, &this->swapchain));
}

void Rendering::RecreateSwapchain()
{
	vkDeviceWaitIdle(this->logicalDevice);

	this->depthImage.Destroy();

	vkFreeCommandBuffers(this->logicalDevice, this->m_commandPool, static_cast<ui32>(this->commandBuffers.size()), this->commandBuffers.data());

	for (size_t i = 0; i < this->framebuffers.size(); i++)
	{
		vkDestroyFramebuffer(this->logicalDevice, this->framebuffers[i], nullptr);
	}
	this->framebuffers.clear();

	for (size_t i = 0; i < this->imageViews.size(); i++)
	{
		vkDestroyImageView(this->logicalDevice, this->imageViews[i], nullptr);
	}
	this->imageViews.clear();
	this->swapchainImages.clear();

	vkDestroyRenderPass(this->logicalDevice, this->renderpass, nullptr);
	
	VkSwapchainKHR oldSwapchain = this->swapchain;

	this->CreateSwapChain();
	this->CreateImageViews();
	this->CreateRenderpass();
	this->CreateDepthImage();
	this->CreateFramebuffers();
	this->CreateCommandbuffer();
	this->RecordCommands();

	vkDestroySwapchainKHR(this->logicalDevice, oldSwapchain, nullptr);
}

void Rendering::CreateImageViews(void)
{
	ui32 swapchainImageCount = 0;
	vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &swapchainImageCount, nullptr);

	this->swapchainImages.resize(swapchainImageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &swapchainImageCount, this->swapchainImages.data()));

	this->imageViews.resize(swapchainImageCount);

	for (ui32 i = 0; i < swapchainImageCount; i++)
	{
		this->CreateImageView(this->swapchainImages[i], VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, this->imageViews[i]);
	}
}

void Rendering::CreateShaderModules()
{
	std::vector<byte> vertexCode = GetBuffer(RenderingBuffer::VERTEX); // TODO if shader dont work.
	std::vector<byte> fragmentCode = GetBuffer(RenderingBuffer::FRAGMENT);

	this->CreateShaderModule(vertexCode, &this->vertexModule);
	this->CreateShaderModule(fragmentCode, &this->fragmentModule);
}

void Rendering::CreateShaderModule(const std::vector<byte>& code, VkShaderModule* shaderModule)
{
	VkShaderModuleCreateInfo shaderInfo;
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.pNext = nullptr;
	shaderInfo.flags = 0;
	shaderInfo.codeSize = code.size() % 4 + code.size();
	shaderInfo.pCode = (ui32*)code.data();

	VK_CHECK(vkCreateShaderModule(this->logicalDevice, &shaderInfo, nullptr, shaderModule));
}

void Rendering::CreateDescriptorSetLayout()
{
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

	VkDescriptorSetLayoutBinding vertexSetLayoutBinding;
	vertexSetLayoutBinding.binding = 0;
	vertexSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vertexSetLayoutBinding.descriptorCount = 1;
	vertexSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vertexSetLayoutBinding.pImmutableSamplers = nullptr;

	descriptorSetLayoutBindings.push_back(vertexSetLayoutBinding);

	VkDescriptorSetLayoutBinding samplerSetLayoutBinding;
	samplerSetLayoutBinding.binding = 1;
	samplerSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerSetLayoutBinding.descriptorCount = 1;
	samplerSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerSetLayoutBinding.pImmutableSamplers = nullptr;

	descriptorSetLayoutBindings.push_back(samplerSetLayoutBinding);

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo;
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.pNext = nullptr;
	descriptorSetLayoutInfo.flags = 0;
	descriptorSetLayoutInfo.bindingCount = static_cast<ui32>(descriptorSetLayoutBindings.size());
	descriptorSetLayoutInfo.pBindings = descriptorSetLayoutBindings.data();

	VK_CHECK(vkCreateDescriptorSetLayout(this->logicalDevice, &descriptorSetLayoutInfo, nullptr, &this->descriptorSetLayout));
}

void Rendering::CreateDescriptorPool()
{
	std::vector<VkDescriptorPoolSize> descriptorPoolSizes;

	VkDescriptorPoolSize uniformPoolSize;
	uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformPoolSize.descriptorCount = 1;

	descriptorPoolSizes.push_back(uniformPoolSize);

	VkDescriptorPoolSize samplerPoolSize;
	samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerPoolSize.descriptorCount = 1;

	descriptorPoolSizes.push_back(samplerPoolSize);

	VkDescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.pNext = nullptr;
	descriptorPoolInfo.flags = 0;
	descriptorPoolInfo.maxSets = 1;
	descriptorPoolInfo.poolSizeCount = static_cast<ui32>(descriptorPoolSizes.size());
	descriptorPoolInfo.pPoolSizes = descriptorPoolSizes.data();

	VK_CHECK(vkCreateDescriptorPool(this->logicalDevice, &descriptorPoolInfo, nullptr, &this->descriptorPool));
}

void Rendering::CreateDescriptorSet()
{
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = this->descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &this->descriptorSetLayout;

	VK_CHECK(vkAllocateDescriptorSets(this->logicalDevice, &descriptorSetAllocateInfo, &this->descriptorSet));

	std::vector<VkWriteDescriptorSet> descriptorSetWrites;

	VkDescriptorBufferInfo descriptorBufferInfo;
	descriptorBufferInfo.buffer = this->uniformBuffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = sizeof(Math::Mat4x4);

	VkWriteDescriptorSet descriptorWrite;
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.pNext = nullptr;
	descriptorWrite.dstSet = this->descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pBufferInfo = &descriptorBufferInfo;
	descriptorWrite.pTexelBufferView = nullptr;

	descriptorSetWrites.push_back(descriptorWrite);

	VkDescriptorImageInfo descriptorSamplerInfo;
	descriptorSamplerInfo.sampler = this->texture.GetSampler();
	descriptorSamplerInfo.imageView = this->texture.GetImageView();
	descriptorSamplerInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet descriptorSamplerWrite;
	descriptorSamplerWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorSamplerWrite.pNext = nullptr;
	descriptorSamplerWrite.dstSet = this->descriptorSet;
	descriptorSamplerWrite.dstBinding = 0;
	descriptorSamplerWrite.dstArrayElement = 0;
	descriptorSamplerWrite.descriptorCount = 1;
	descriptorSamplerWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSamplerWrite.pImageInfo = &descriptorSamplerInfo;
	descriptorSamplerWrite.pBufferInfo = nullptr;
	descriptorSamplerWrite.pTexelBufferView = nullptr;

	descriptorSetWrites.push_back(descriptorSamplerWrite);

	vkUpdateDescriptorSets(this->logicalDevice, static_cast<ui32>(descriptorSetWrites.size()), descriptorSetWrites.data(), 0, nullptr);
}

void Rendering::CreatePipeline()
{
	this->CreateShaderModules();

	//SHADERSTAGE INFOS
	VkPipelineShaderStageCreateInfo vertShaderStageInfo;
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.pNext = nullptr;
	vertShaderStageInfo.flags = 0; //TODO
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = this->vertexModule;
	vertShaderStageInfo.pName = "main";
	vertShaderStageInfo.pSpecializationInfo = nullptr; //Constant variables

	VkPipelineShaderStageCreateInfo fragShaderStageInfo;
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.pNext = nullptr;
	fragShaderStageInfo.flags = 0; //TODO
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = this->fragmentModule;
	fragShaderStageInfo.pName = "main";
	fragShaderStageInfo.pSpecializationInfo = nullptr; //Constant variables

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindings = Vertex::GetBindingDescription();
	auto attributes = Vertex::GetAttributeDescriptions();

	//VERTEX INPUT
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pNext = nullptr;
	vertexInputInfo.flags = 0;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindings; //for mesh instancing
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<ui32>(attributes.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributes.data(); //For data

	//INPUT ASSEMBLY
	VkPipelineInputAssemblyStateCreateInfo inputAsseblyInfo;
	inputAsseblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAsseblyInfo.pNext = nullptr;
	inputAsseblyInfo.flags = 0;
	inputAsseblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAsseblyInfo.primitiveRestartEnable = VK_FALSE;

	//VIEWPORT
	VkViewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(this->surfaceCapabilities.currentExtent.width);
	viewport.height = static_cast<float>(this->surfaceCapabilities.currentExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent = { this->surfaceCapabilities.currentExtent.width, this->surfaceCapabilities.currentExtent.height };

	VkPipelineViewportStateCreateInfo viewportInfo;
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.pNext = nullptr;
	viewportInfo.flags = 0;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &viewport;
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
	rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

	VkDynamicState dynamicStates[]
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

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
	pipelineLayoutInfo.setLayoutCount = 1; //TODO for variables that should be changed during runtime.
	pipelineLayoutInfo.pSetLayouts = &this->descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	VK_CHECK(vkCreatePipelineLayout(this->logicalDevice, &pipelineLayoutInfo, nullptr, &this->pipelineLayout));

	this->CreateRenderpass();

	//CREATE PIPELINE
	VkGraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = 2;
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
	pipelineInfo.basePipelineIndex = -10; //invalid index

	VK_CHECK(vkCreateGraphicsPipelines(this->logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->pipeline));
}

void Rendering::CreateRenderpass()
{

	//ATTACHMENTS
	VkAttachmentDescription attachmentDescription;
	attachmentDescription.flags = 0;
	attachmentDescription.format = VK_FORMAT_B8G8R8A8_UNORM;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachmentReference;
	attachmentReference.attachment = 0;
	attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthDescription = this->depthImage.GetAttachmentDescription(this->physicalDevice);

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

	VkSubpassDependency subpassDependency;
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependency.dependencyFlags = 0;

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

	VK_CHECK(vkCreateRenderPass(this->logicalDevice, &renderpassInfo, nullptr, &this->renderpass));
}

void Rendering::CreateFramebuffers()
{
	this->framebuffers.resize(this->swapchainImages.size());

	for (size_t i = 0; i < this->swapchainImages.size(); i++)
	{
		std::vector<VkImageView> attachmentViews;
		attachmentViews.push_back(this->imageViews[i]);
		attachmentViews.push_back(this->depthImage.GetDepthImageView());
		VkFramebufferCreateInfo framebufferInfo;
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.flags = 0; //TODO
		framebufferInfo.renderPass = this->renderpass;
		framebufferInfo.attachmentCount = static_cast<ui32>(attachmentViews.size());
		framebufferInfo.pAttachments = attachmentViews.data();
		framebufferInfo.width = this->surfaceCapabilities.currentExtent.width;
		framebufferInfo.height = this->surfaceCapabilities.currentExtent.height;
		framebufferInfo.layers = 1;

		VK_CHECK(vkCreateFramebuffer(this->logicalDevice, &framebufferInfo, nullptr, &(this->framebuffers[i])));
	}
}

void Rendering::CreateCommandPool(void)
{
	VkCommandPoolCreateInfo commandPoolInfo;
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;
	commandPoolInfo.flags = 0; //TODO
	commandPoolInfo.queueFamilyIndex = this->indexOfGraphicsQueue;

	VK_CHECK(vkCreateCommandPool(this->logicalDevice, &commandPoolInfo, nullptr, &this->m_commandPool));
}

void Rendering::CreateCommandbuffer()
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = this->m_commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = static_cast<ui32>(this->swapchainImages.size());

	this->commandBuffers.resize(this->swapchainImages.size());
	VK_CHECK(vkAllocateCommandBuffers(this->logicalDevice, &commandBufferAllocateInfo, this->commandBuffers.data()));
}

void Rendering::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsageFlags, VkBuffer& buffer, VkMemoryPropertyFlags memoryFlags, VkDeviceMemory& memory)
{
	VkBufferCreateInfo bufferInfo;
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.flags = 0;
	bufferInfo.size = size;
	bufferInfo.usage = bufferUsageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.queueFamilyIndexCount = 0;
	bufferInfo.pQueueFamilyIndices = nullptr;

	VK_CHECK(vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &buffer));

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(this->logicalDevice, buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = this->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryFlags);

	VK_CHECK(vkAllocateMemory(this->logicalDevice, &memoryAllocateInfo, nullptr, &memory));

	VK_CHECK(vkBindBufferMemory(this->logicalDevice, buffer, memory, 0));
}

void Rendering::CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
	VkCommandPool transferCommandPool = VK_NULL_HANDLE;
	
	VkCommandBufferAllocateInfo commandBufferInfo;
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.commandPool = this->m_commandPool;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount = 1;

	if (this->indexOfTransferQueue != this->indexOfGraphicsQueue)
	{

		VkCommandPoolCreateInfo commandPoolInfo;
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.pNext = nullptr;
		commandPoolInfo.flags = 0; 
		commandPoolInfo.queueFamilyIndex = this->indexOfTransferQueue;

		VK_CHECK(vkCreateCommandPool(this->logicalDevice, &commandPoolInfo, nullptr, &transferCommandPool));

		commandBufferInfo.commandPool = transferCommandPool;
	}

	VkCommandBuffer commandBuffer;
	VK_CHECK(vkAllocateCommandBuffers(this->logicalDevice, &commandBufferInfo, &commandBuffer));

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

	VkBufferCopy bufferCopy;
	bufferCopy.srcOffset = 0;
	bufferCopy.dstOffset = 0;
	bufferCopy.size = size;

	vkCmdCopyBuffer(commandBuffer, src, dst, 1, &bufferCopy);

	VK_CHECK(vkEndCommandBuffer(commandBuffer));

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

	if (this->transferQueue != VK_NULL_HANDLE)
	{
		VK_CHECK(vkQueueSubmit(this->transferQueue, 1, &submitInfo, VK_NULL_HANDLE));
		vkQueueWaitIdle(this->transferQueue);
	}
	else
	{
		VK_CHECK(vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE)); //Transfer bit is included with graphics bit.
		vkQueueWaitIdle(this->graphicsQueue);
	}

	if (this->indexOfTransferQueue != this->indexOfGraphicsQueue)
	{
		vkFreeCommandBuffers(this->logicalDevice, transferCommandPool, 1, &commandBuffer);
		vkDestroyCommandPool(this->logicalDevice, transferCommandPool, nullptr);
	}
	else
	{
		vkFreeCommandBuffers(this->logicalDevice, this->m_commandPool, 1, &commandBuffer);
	}

}

template <typename T>
void Rendering::CreateBufferOnGPU(std::vector<T> data, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory)
{
	VkDeviceSize bufferSize = sizeof(T) * data.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), stagingBufferMemory);

	void* pointerData;
	vkMapMemory(this->logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &pointerData);
	memcpy(pointerData, data.data(), bufferSize);
	vkUnmapMemory(this->logicalDevice, stagingBufferMemory);

	this->CreateBuffer(bufferSize, (usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT), buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory);

	this->CopyBuffer(stagingBuffer, buffer, bufferSize);

	vkDestroyBuffer(this->logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(this->logicalDevice, stagingBufferMemory, nullptr);
}

void Rendering::CreateUniformbuffer()
{
	VkDeviceSize bufferSize = sizeof(Math::Mat4x4);
	this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, this->uniformBuffer, (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), this->uniformBufferMemory);
}

void Rendering::LoadTexture(void)
{
	this->texture.Load("D:/Documents/Code/Engine/GameEngine/textures/texture.jpg"); //TODO
	this->texture.Upload(this->m_commandPool, this->graphicsQueue);

}

void Rendering::LoadModels(void)
{
	this->mesh.Create("D:/Documents/Code/Engine/GameEngine/meshes/dragon.obj"); //TODO
}

void Rendering::CreateVertexbuffers()
{
	this->CreateBufferOnGPU(this->mesh.GetVertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, this->vertexBuffers[0], this->vertexBufferMemories[0]);
}

void Rendering::CreateIndexbuffers()
{
	this->CreateBufferOnGPU(this->mesh.GetIndices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, this->indexBuffers[0], this->indexBufferMemories[0]);
}

void Rendering::RecordCommands()
{
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	for (size_t i = 0; i < this->swapchainImages.size(); i++)
	{
		VK_CHECK(vkBeginCommandBuffer(this->commandBuffers[i], &commandBufferBeginInfo));

		VkClearValue clearValue = { };
		clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};

		VkClearValue depthClearValue = {1.0f, 0 };

		std::vector <VkClearValue> clearValues;
		clearValues.push_back(clearValue);
		clearValues.push_back(depthClearValue);

		VkRenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = this->renderpass;
		renderPassBeginInfo.framebuffer = this->framebuffers[i];
		renderPassBeginInfo.renderArea.extent = this->surfaceCapabilities.currentExtent;
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.clearValueCount = static_cast<ui32>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(this->commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(this->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(this->surfaceCapabilities.currentExtent.width);
		viewport.height = static_cast<float>(this->surfaceCapabilities.currentExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor;
		scissor.offset = { 0, 0 };
		scissor.extent = { this->surfaceCapabilities.currentExtent.width, this->surfaceCapabilities.currentExtent.height };

		vkCmdSetViewport(this->commandBuffers[i], 0, 1, &viewport);
		vkCmdSetScissor(this->commandBuffers[i], 0, 1, &scissor);

		VkDeviceSize offsets[] = {0};

		vkCmdBindVertexBuffers(this->commandBuffers[i], 0, 1, this->vertexBuffers.data(), offsets);

		vkCmdBindIndexBuffer(this->commandBuffers[i],  this->indexBuffers[0], 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(this->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayout, 0, 1, &this->descriptorSet, 0, nullptr);
		vkCmdDrawIndexed(this->commandBuffers[i], static_cast<ui32>(this->mesh.GetIndices().size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(this->commandBuffers[i]);
		
		VK_CHECK(vkEndCommandBuffer(this->commandBuffers[i]));
	}

}

void Rendering::CreateSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo;
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = nullptr;
	semaphoreInfo.flags = 0;

	VK_CHECK(vkCreateSemaphore(this->logicalDevice, &semaphoreInfo, nullptr, &this->imageAvailableSemaphore));
	VK_CHECK(vkCreateSemaphore(this->logicalDevice, &semaphoreInfo, nullptr, &this->renderingFinishedSemaphore));

	this->waitStageMask.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
}

void Rendering::CreateDepthImage(void)
{
	this->depthImage.Create(this->physicalDevice, this->m_commandPool, this->graphicsQueue, this->surfaceCapabilities.currentExtent.width, this->surfaceCapabilities.currentExtent.height);
}

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

bool Rendering::isStencil(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat Rendering::GetSupportedFormats(VkPhysicalDevice phyDevice, const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags)
{
	for (VkFormat format : formats)
	{
		if (this->isFormatSupported(phyDevice, format, tiling, featureFlags))
			return format;
	}

	return VkFormat();
}

bool Rendering::isModeSupported(const std::vector<VkPresentModeKHR>& supportedPresentModes, VkPresentModeKHR presentMode)
{
	for (const VkPresentModeKHR& supportedPresentMode : supportedPresentModes)
	{
		if (supportedPresentMode == presentMode)
			return true;
	}

	return false;
}

std::vector<byte> Rendering::GetBuffer(RenderingBuffer bufferType)
{
	std::vector<byte> buffer;

	switch (bufferType)
	{
		case RenderingBuffer::VERTEX:
		{
			WinFile* vertexShaderFile = new WinFile(Application::GetInstancePtr()->GetFilesystem()->FileInDirectory("shader", "vert.spv").c_str());
			buffer.assign(vertexShaderFile->Read(), vertexShaderFile->Read() + vertexShaderFile->GetSize());
			vertexShaderFile->Cleanup();
			break;
		}
		case RenderingBuffer::FRAGMENT:
		{
			WinFile* fragmentShaderFile = new WinFile(Application::GetInstancePtr()->GetFilesystem()->FileInDirectory("shader", "frag.spv").c_str());
			buffer.assign(fragmentShaderFile->Read(), fragmentShaderFile->Read() + fragmentShaderFile->GetSize());
			fragmentShaderFile->Cleanup();
			break;
		}
	}

	return buffer;
}

ui32 Rendering::FindMemoryTypeIndex(ui32 typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &deviceMemoryProperties);

	for (ui32 i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw;

	return 0;
}

VkDevice Rendering::GetLogicalDevice(void)
{
	return this->logicalDevice;
}

bool Rendering::GetInitStatus()
{
	return this->initialized;
}

void Rendering::CreateImage(ui32 width, ui32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage & image, VkDeviceMemory & imageMemory)
{
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

	VK_CHECK(vkCreateImage(this->logicalDevice, &imageInfo, nullptr, &image));

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(this->logicalDevice, image, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = Rendering::GetInstancePtr()->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, propertyFlags);

	VK_CHECK(vkAllocateMemory(this->logicalDevice, &memoryAllocateInfo, nullptr, &imageMemory));

	vkBindImageMemory(this->logicalDevice, image, imageMemory, 0);

}

void Rendering::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView)
{
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

void Rendering::Cleanup()
{
	VK_CHECK(vkDeviceWaitIdle(this->logicalDevice));

	this->depthImage.Destroy();

	vkDestroyDescriptorSetLayout(this->logicalDevice, this->descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(this->logicalDevice, this->descriptorPool, nullptr);

	vkFreeMemory(this->logicalDevice, this->uniformBufferMemory, nullptr);
	vkDestroyBuffer(this->logicalDevice, this->uniformBuffer, nullptr);

	for (size_t i = 0; i < this->gameObjects.size(); i++)
	{
		vkFreeMemory(this->logicalDevice, this->indexBufferMemories[i], nullptr);
		vkDestroyBuffer(this->logicalDevice, this->indexBuffers[i], nullptr);
	
		vkFreeMemory(this->logicalDevice, this->vertexBufferMemories[i], nullptr);
		vkDestroyBuffer(this->logicalDevice, this->vertexBuffers[i], nullptr);
	}

	this->texture.Release();

	this->waitStageMask.clear();
	vkDestroySemaphore(this->logicalDevice, this->imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(this->logicalDevice, this->renderingFinishedSemaphore, nullptr);

	vkFreeCommandBuffers(this->logicalDevice, this->m_commandPool, static_cast<ui32>(this->commandBuffers.size()), this->commandBuffers.data());
	vkDestroyCommandPool(this->logicalDevice, this->m_commandPool, nullptr);

	for (size_t i = 0; i < this->framebuffers.size(); i++)
	{
		vkDestroyFramebuffer(this->logicalDevice, this->framebuffers[i], nullptr);
	}
	this->framebuffers.clear();
	
	for (size_t i = 0; i < this->imageViews.size(); i++)
	{
		vkDestroyImageView(this->logicalDevice, this->imageViews[i], nullptr);
	}
	this->imageViews.clear();
	this->swapchainImages.clear();

	vkDestroyPipeline(this->logicalDevice, this->pipeline, nullptr);
	vkDestroyRenderPass(this->logicalDevice, this->renderpass, nullptr);
	vkDestroyPipelineLayout(this->logicalDevice, this->pipelineLayout, nullptr);
	vkDestroyShaderModule(this->logicalDevice, this->vertexModule, nullptr);
	vkDestroyShaderModule(this->logicalDevice, this->fragmentModule, nullptr);
	vkDestroySwapchainKHR(this->logicalDevice, this->swapchain, nullptr);
	vkDestroyDevice(this->logicalDevice, nullptr);
	vkDestroySurfaceKHR(this->vkInstance, this->surface, nullptr);
	vkDestroyInstance(this->vkInstance, nullptr);
} 
