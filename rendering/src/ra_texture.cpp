#include "ra_texture.h"
#include "ra_rendering.h"
#include "ra_gameobject.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ra_stb_image.h"
#include "ra_vkcheck.h"

Texture::Texture()
{
}

void Texture::Load(const char* texturePath)
{
	this->pixels = stbi_load(texturePath, &this->width, &this->height, &this->channels, STBI_rgb_alpha);
	this->logicalDeviceHandle = Rendering::GetInstancePtr()->GetLogicalDevice();

	std::string tempName(texturePath);

	tempName = tempName.substr(tempName.find_last_of("/") + 1);

	this->name = tempName;
	this->path = texturePath;

	if (this->pixels == nullptr)
		throw;

	this->loaded = true;
}

void Texture::Upload(VkCommandPool commandPool, VkQueue queue)
{
	//TODO
	if (!this->loaded)
		throw printf("Image not loaded!");
	else if (this->uploaded)
		throw printf("Image already uploaded");

	VkDeviceSize imageSize = static_cast<ui64>(this->GetByteSize());
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	Rendering::GetInstancePtr()->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);

	void* data;
	vkMapMemory(this->logicalDeviceHandle, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, this->pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(this->logicalDeviceHandle, stagingBufferMemory);

	Rendering::GetInstancePtr()->CreateImage(static_cast<ui32>(this->GetWidth()), static_cast<ui32>(this->GetHeight()), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->image, this->imageMemory);

	Rendering::GetInstancePtr()->ChangeLayout(commandPool, queue, this->image, VK_FORMAT_R8G8B8A8_UNORM, this->imageLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	this->imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	this->TransferImage(commandPool, queue, stagingBuffer);
	Rendering::GetInstancePtr()->ChangeLayout(commandPool, queue, this->image, VK_FORMAT_R8G8B8A8_UNORM, this->imageLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	this->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	vkDestroyBuffer(this->logicalDeviceHandle, stagingBuffer, nullptr);
	vkFreeMemory(this->logicalDeviceHandle, stagingBufferMemory, nullptr);

	Rendering::GetInstancePtr()->CreateImageView(this->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, this->imageView);

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

	this->uploaded = true;
}

void Texture::TransferImage(VkCommandPool commandPool, VkQueue queue, VkBuffer buffer)
{
	VkCommandBuffer commandBuffer;
	
	Rendering::GetInstancePtr()->BeginRecording(commandBuffer, commandPool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

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

	Rendering::GetInstancePtr()->StopRecording(queue, commandBuffer, commandPool);
}

void Texture::Release()
{
	if (this->loaded)
	{
		stbi_image_free(this->pixels);
		this->loaded = false;
	}

	if (this->uploaded)
	{
		vkDestroySampler(this->logicalDeviceHandle, this->sampler, nullptr);
		vkDestroyImageView(this->logicalDeviceHandle, this->imageView, nullptr);
		vkDestroyImage(this->logicalDeviceHandle, this->image, nullptr);
		vkFreeMemory(this->logicalDeviceHandle, this->imageMemory, nullptr);

		this->logicalDeviceHandle = VK_NULL_HANDLE;
		this->uploaded = false;
	}
}

byte* Texture::GetPixels()
{
	return this->pixels;
}

int Texture::GetWidth()
{
	return this->width;
}

int Texture::GetHeight()
{
	return this->height;
}

int Texture::GetChannels()
{
	return this->channels;
}

int Texture::GetByteSize()
{
	return this->width * this->height * 4;
}

const char* Texture::GetPath()
{
	return this->path;
}

std::string Texture::GetName()
{
	return this->name;
}

bool Texture::isLoaded()
{
	return this->loaded;
}

VkSampler Texture::GetSampler()
{
	return this->sampler;
}

VkImageView Texture::GetImageView()
{
	return this->imageView;
}
