
//EXTERNAL INCLUDES
#include <vector>
//INTERNAL INCLUDES
#include "ra_depthimage.h"
#include "ra_rendering.h"

//Create a depth image by creating the image, image view and chaning the layout of the image
void DepthImage::Create(VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, ui32 width, ui32 height)
{
	//If it has already created
	if (this->created)
		return;

	//Create the depth format by getting the correct one.
	VkFormat depthFormat = this->FindDepthFormat(physicalDevice);

	//Create image and image view with specific flags for depth images
	Rendering::GetInstancePtr()->CreateImage(width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->image, this->imageMemory);
	Rendering::GetInstancePtr()->CreateImageView(this->image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, this->imageView);

	//Finally change the layout
	Rendering::GetInstancePtr()->ChangeLayout(commandPool, queue, this->image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	//Creating finished
	this->created = true;
}
//Destroy the depth image
void DepthImage::Destroy()
{
	//Only if it was created
	if (created)
	{
		//Destroy the image view and the image, and free the memory again
		vkDestroyImageView(Rendering::GetInstancePtr()->GetLogicalDevice(), this->imageView, nullptr);
		vkDestroyImage(Rendering::GetInstancePtr()->GetLogicalDevice(), this->image, nullptr);
		vkFreeMemory(Rendering::GetInstancePtr()->GetLogicalDevice(), this->imageMemory, nullptr);
		
		//Set handles to null
		this->image = VK_NULL_HANDLE;
		this->imageMemory = VK_NULL_HANDLE;
		this->imageView = VK_NULL_HANDLE;
		
		//Destruction complete	
		this->created = false;
	}
}


//Get the create info of the depth image
VkPipelineDepthStencilStateCreateInfo DepthImage::GetDepthInfo()
{
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.pNext = nullptr;
	depthStencilInfo.flags = 0;
	depthStencilInfo.depthTestEnable = VK_TRUE;
	depthStencilInfo.depthWriteEnable = VK_TRUE;
	depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilInfo.stencilTestEnable = VK_FALSE;
	depthStencilInfo.front = { };
	depthStencilInfo.back = { };
	depthStencilInfo.minDepthBounds = 0.0f;
	depthStencilInfo.maxDepthBounds = 1.0f;

	return depthStencilInfo;
}
//Create and return the attachment discription for the depth format
VkAttachmentDescription DepthImage::GetAttachmentDescription(VkPhysicalDevice physicalDevice)
{
	VkAttachmentDescription depthDescription;
	depthDescription.flags = 0;
	depthDescription.format = this->FindDepthFormat(physicalDevice);
	depthDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	depthDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	return depthDescription;
}
//Get depth image view
VkImageView DepthImage::GetDepthImageView()
{
	return this->imageView;
}


//Finding the depth format and return it. 
VkFormat DepthImage::FindDepthFormat(VkPhysicalDevice physicalDevice)
{
	//List of formats that fit
	std::vector<VkFormat> supportedFormats = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT };

	//Support the one that fits best
	return Rendering::GetInstancePtr()->GetSupportedFormats(physicalDevice, supportedFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}