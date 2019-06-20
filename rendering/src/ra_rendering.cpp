
#define VK_USE_PLATFORM_WIN32_KHR
// EXTERNAL INCLUDES
#include <vulkan/vulkan.h>

// INTERNAL INCLUDES
#include "ra_rendering.h"
#include "ra_vkcheck.h"
#include "ra_types.h"
#include "ra_utils.h"
#include "ra_window.h"
#include "filesystem/ra_filesystem.h"
#include "filesystem/ra_winfile.h"

void Rendering::Initialize(const char* applicationName, ui32 applicationVersion)
{
	this->CreateInstance(applicationName, applicationVersion);
	this->CreatePhysicalDevice();
	this->CreateSurface();
	this->CreateLogicalDevice();
	this->CreateSwapChain();
	this->CreateImageViews();
	this->CreateShaderModules();
	this->CreatePipeline();
}

void Rendering::CreateInstance(const char* applicationName, ui32 applicationVersion)
{
	const std::vector<const char*> instanceValidationLayers =
	{
		"VK_LAYER_LUNARG_standard_validation"
	};

	const std::vector<const char*> instanceExtensions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = applicationName;
	appInfo.applicationVersion = applicationVersion;
	appInfo.pEngineName = "RA Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0; //TODO
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = static_cast<ui32>(instanceValidationLayers.size());
	instanceInfo.ppEnabledLayerNames = instanceValidationLayers.data();
	instanceInfo.enabledExtensionCount = static_cast<ui32>(instanceExtensions.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data(); 

	VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &this->instance));
}

void Rendering::CreatePhysicalDevice()
{
	ui32 physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, physicalDevices.data()));

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
			rating += 300;

		ui32 queueFamiliyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamiliyCount, nullptr);

		std::vector<VkQueueFamilyProperties> familiyProperties(queueFamiliyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamiliyCount, familiyProperties.data());

		for (int i = 0; i < familiyProperties.size(); i++)
		{
			if ((familiyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				rating += 100;
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

void Rendering::CreateLogicalDevice()
{
	float queuePriority = 1.0f;

	VkDeviceQueueCreateInfo queueInfo;
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = nullptr;
	queueInfo.flags = 0;
	queueInfo.queueFamilyIndex = 0;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures enabledFeatures = { };
	
	//const std::vector<const char*> deviceValidationLayers =
	//{
	//};

	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDeviceCreateInfo deviceInfo;
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.flags = 0;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledLayerNames = nullptr;
	deviceInfo.enabledExtensionCount = static_cast<ui32>(deviceExtensions.size());
	deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
	deviceInfo.pEnabledFeatures = &enabledFeatures;

	VK_CHECK(vkCreateDevice(this->physicalDevice, &deviceInfo, nullptr, &this->logicalDevice));

	VkQueue queue = { };
	vkGetDeviceQueue(this->logicalDevice, 0, 0, &queue);
}

void Rendering::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo;
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0; //TODO
	surfaceInfo.hinstance = GetModuleHandle(NULL);
	surfaceInfo.hwnd = Window::GetInstancePtr()->GetHandle();

	VK_CHECK(vkCreateWin32SurfaceKHR(this->instance, &surfaceInfo, nullptr, &this->surface));
}

void Rendering::CreateSwapChain(void)
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDevice, this->surface, &surfaceCapabilities));

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
	swapchainInfo.minImageCount = surfaceCapabilities.maxImageCount >= 3 ? 3 : 2;
	swapchainInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainInfo.imageExtent.height = surfaceCapabilities.currentExtent.height;
	swapchainInfo.imageExtent.width = surfaceCapabilities.currentExtent.width;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //Change if more than one queue
	swapchainInfo.queueFamilyIndexCount = 0; //TODO if more queues
	swapchainInfo.pQueueFamilyIndices = nullptr; //TODO if more queuess
	swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.presentMode = this->isModeSupported(presentModes, VK_PRESENT_MODE_MAILBOX_KHR) ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

	VK_CHECK(vkCreateSwapchainKHR(this->logicalDevice, &swapchainInfo, nullptr, &this->swapchain));
}

void Rendering::CreateImageViews(void)
{
	ui32 swapchainImageCount = 0;
	vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &swapchainImageCount, nullptr);

	std::vector<VkImage> swapchainImages;

	swapchainImages.resize(swapchainImageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &swapchainImageCount, swapchainImages.data()));

	this->imageViews.resize(swapchainImageCount);

	for (ui32 i = 0; i < swapchainImageCount; i++)
	{
		VkImageViewCreateInfo imageViewInfo;
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.pNext = nullptr;
		imageViewInfo.flags = 0; //TODO
		imageViewInfo.image = swapchainImages[i];
		imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.levelCount = 1;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(this->logicalDevice, &imageViewInfo, nullptr, &this->imageViews[i]));
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

void Rendering::CreatePipeline()
{
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

	//VERTEX INPUT
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pNext = nullptr;
	vertexInputInfo.flags = 0;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr; //for mesh instancing
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr; //For data

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
	viewport.width = static_cast<float>(Window::GetInstancePtr()->GetWidth());
	viewport.height = static_cast<float>(Window::GetInstancePtr()->GetHeight());
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent = { Window::GetInstancePtr()->GetWidth(), Window::GetInstancePtr()->GetHeight() };

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

	//BlendMode
	VkPipelineColorBlendAttachmentState colorBlendState;
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
			WinFile* vertexShaderFile = new WinFile(Window::GetInstancePtr()->GetFileSystem()->FileInDirectory("shader", "vert.spv").c_str());
			buffer.assign(vertexShaderFile->Read(), vertexShaderFile->Read() + vertexShaderFile->GetSize());
			vertexShaderFile->Cleanup();
			break;
		}
		case RenderingBuffer::FRAGMENT:
		{
			WinFile* fragmentShaderFile = new WinFile(Window::GetInstancePtr()->GetFileSystem()->FileInDirectory("shader", "frag.spv").c_str());
			buffer.assign(fragmentShaderFile->Read(), fragmentShaderFile->Read() + fragmentShaderFile->GetSize());
			fragmentShaderFile->Cleanup();
			break;
		}
	}

	return buffer;
}

void Rendering::Cleanup()
{
	VK_CHECK(vkDeviceWaitIdle(this->logicalDevice));

	for (ui32 i = 0; i < this->imageViews.size(); i++)
	{
		vkDestroyImageView(this->logicalDevice, this->imageViews[i], nullptr);
	}

	this->imageViews.clear();

	vkDestroyShaderModule(this->logicalDevice, this->vertexModule, nullptr);
	vkDestroyShaderModule(this->logicalDevice, this->fragmentModule, nullptr);
	vkDestroySwapchainKHR(this->logicalDevice, this->swapchain, nullptr);
	vkDestroyDevice(this->logicalDevice, nullptr);
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyInstance(this->instance, nullptr);

	Window::GetInstancePtr()->GetFileSystem()->Cleanup();
} 
