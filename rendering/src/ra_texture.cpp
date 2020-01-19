
#define STB_IMAGE_IMPLEMENTATION //Define to use texture sampler

//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_texture.h"
#include "ra_stb_image.h"
#include "ra_rendering.h"
#include "ra_gameobject.h"
#include "ra_vkcheck.h"

//Load the texture, only possible if values have been set
void Texture::Load()
{
	//If already loaded
	if (this->loaded)
		return;

	//Get pixel data using the path, references to values that will be set by function and using the texture sampler
	this->pixels = stbi_load(this->path.c_str(), &this->width, &this->height, &this->channels, STBI_rgb_alpha);

	//If pixels are null throw
	if (this->pixels == nullptr)
		throw;
	
	//Set handle to logical device
	this->logicalDeviceHandle = Rendering::GetInstancePtr()->GetLogicalDevice();

	//Convert path to string
	std::string tempName(this->path.c_str());

	//Get only the name of the texture
	tempName = tempName.substr(tempName.find_last_of("/") + 1);
	
	//Set that name
	this->name = tempName;

	//Loading completed
	this->loaded = true;
}
//Upload the texture, only possible if values have been set
void Texture::Upload()
{
	//Get command pool and queue
	VkCommandPool commandPool = Rendering::GetInstancePtr()->GetCommandPool();
	VkQueue queue = Rendering::GetInstancePtr()->GetGraphicsQueue();

	//If not loaded yet or already uploaded return.
	if (!this->loaded)
		return;
	else if (this->uploaded)
		return;

	//Setup image size, staging buffer with memory
	VkDeviceSize imageSize = static_cast<ui64>(this->GetByteSize());
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	//Create the image buffer
	Rendering::GetInstancePtr()->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);

	//Map the memory of the staging buffer memory, copy the pixel data to it and unmap it again.
	void* data;
	vkMapMemory(this->logicalDeviceHandle, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, this->pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(this->logicalDeviceHandle, stagingBufferMemory);

	//Create image with the image and image memory (are being set by this function), width, height and specific flags
	Rendering::GetInstancePtr()->CreateImage(static_cast<ui32>(this->GetWidth()), static_cast<ui32>(this->GetHeight()),
		VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, 
		(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->image, this->imageMemory);

	//Change the layout to the image layout
	Rendering::GetInstancePtr()->ChangeLayout(commandPool, queue, this->image, VK_FORMAT_R8G8B8A8_UNORM, this->imageLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	
	//Change local layout
	this->imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	//Transfer the image using the staging buffer
	this->TransferImage(commandPool, queue, stagingBuffer);
	
	//Change the layout again
	Rendering::GetInstancePtr()->ChangeLayout(commandPool, queue, this->image, VK_FORMAT_R8G8B8A8_UNORM, this->imageLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	
	//Change local layout
	this->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Destroy and free staging buffer/memory
	vkDestroyBuffer(this->logicalDeviceHandle, stagingBuffer, nullptr);
	vkFreeMemory(this->logicalDeviceHandle, stagingBufferMemory, nullptr);

	//Create image view
	Rendering::GetInstancePtr()->CreateImageView(this->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, this->imageView);

	//Create sampler
	VkSamplerCreateInfo samplerInfo;
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.pNext = nullptr;
	samplerInfo.flags = 0;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	VK_CHECK(vkCreateSampler(this->logicalDeviceHandle, &samplerInfo, nullptr, &this->sampler));

	//Uploading complete
	this->uploaded = true;
}
//Release the texture
void Texture::Release()
{
	//If its loaded
	if (this->loaded)
	{
		//Free the pixel of the image
		stbi_image_free(this->pixels);
		//Unload
		this->loaded = false;
	}
	//If uploaded
	if (this->uploaded)
	{
		//Destroy sampler, image view, image and free the image memory
		vkDestroySampler(this->logicalDeviceHandle, this->sampler, nullptr);
		vkDestroyImageView(this->logicalDeviceHandle, this->imageView, nullptr);
		vkDestroyImage(this->logicalDeviceHandle, this->image, nullptr);
		vkFreeMemory(this->logicalDeviceHandle, this->imageMemory, nullptr);
		
		//Set logical device handle to null
		this->logicalDeviceHandle = VK_NULL_HANDLE;

		//Unupload
		this->uploaded = false;
	}
}


//Return the sampler
VkSampler Texture::GetSampler()
{
	return this->sampler;
}
//Return the image view
VkImageView Texture::GetImageView()
{
	return this->imageView;
}

//Get the name
std::string Texture::GetName()
{
	return this->name;
}
//Get path as a string reference
std::string& Texture::GetPath()
{
	return this->path;
}

//Return the pixels
byte* Texture::GetPixels()
{
	return this->pixels;
}

//Return width of texture
int Texture::GetWidth()
{
	return this->width;
}
//Return height of texture
int Texture::GetHeight()
{
	return this->height;
}
//Return channels of texture
int Texture::GetChannels()
{
	return this->channels;
}
//Return byte size of the whole texture
int Texture::GetByteSize()
{
	return this->width * this->height * 4; // 1 byte == 4 bits
}

//Return if the texture has been loaded
bool Texture::isLoaded()
{
	return this->loaded;
}


//Transfer image 
void Texture::TransferImage(VkCommandPool commandPool, VkQueue queue, VkBuffer buffer)
{
	VkCommandBuffer commandBuffer;
	
	//Begin recording the command buffer
	Rendering::GetInstancePtr()->BeginRecording(commandBuffer, commandPool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Copy buffer to the image
	VkBufferImageCopy imageCopyBuffer;
	imageCopyBuffer.bufferOffset = 0;
	imageCopyBuffer.bufferRowLength = 0;
	imageCopyBuffer.bufferImageHeight = 0;
	imageCopyBuffer.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopyBuffer.imageSubresource.mipLevel = 0;
	imageCopyBuffer.imageSubresource.baseArrayLayer = 0;
	imageCopyBuffer.imageSubresource.layerCount = 1;
	imageCopyBuffer.imageOffset = {0, 0, 0};
	imageCopyBuffer.imageExtent = { static_cast<ui32>(this->width), static_cast<ui32>(this->height), 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, this->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyBuffer);

	//Stop recording the command buffer
	Rendering::GetInstancePtr()->StopRecording(queue, commandBuffer, commandPool);
}