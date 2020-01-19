
#pragma once
//EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
//INTERNAL INCLUDES
#include "ra_types.h"

//Class for the 3D depth layer
class DepthImage
{
public:
	//Create a depth image by creating the image, image view and chaning the layout of the image
	void Create(VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, ui32 width, ui32 height);
	//Destroy the depth image
	void Destroy();


	//Get the create info of the depth image
	VkPipelineDepthStencilStateCreateInfo GetDepthInfo();
	//Create and return the attachment discription for the depth format
	VkAttachmentDescription GetAttachmentDescription(VkPhysicalDevice physicalDevice);
	//Get depth image view
	VkImageView GetDepthImageView();

private:
	//Finding the depth format and return it. 
	VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory imageMemory = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;

	bool created = false;
};