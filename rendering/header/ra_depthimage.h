
#pragma once
#include <vulkan/vulkan.h>
#include "ra_types.h"

class DepthImage
{
public:
	DepthImage();
	void Create(VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, ui32 width, ui32 height);
	void Destroy();

	VkAttachmentDescription GetAttachmentDescription(VkPhysicalDevice physicalDevice);
	VkPipelineDepthStencilStateCreateInfo GetDepthInfo();
	VkImageView GetDepthImageView();
private:
	VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory imageMemory = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;

	bool created = false;
};