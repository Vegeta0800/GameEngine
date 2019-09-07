
#pragma once
#include <vulkan/vulkan.h>
#include "ra_types.h"
#include <string>

class Texture
{
public:
	Texture();

	void Load();
	void Upload();
	void Release();

	byte* GetPixels();

	int GetWidth();
	int GetHeight();
	int GetChannels();
	int GetByteSize();

	std::string GetName();
	std::string& GetPath();

	bool isLoaded();

	VkSampler GetSampler();
	VkImageView GetImageView();
private:
	void ChangeLayout(VkCommandPool commandPool, VkQueue queue, VkImageLayout layout);
	void TransferImage(VkCommandPool commandPool, VkQueue queue, VkBuffer buffer);

	int width = 0;
	int height = 0;
	int channels = 0;

	byte* pixels = nullptr;
	std::string name;

	std::string path;

	bool loaded = false;
	bool uploaded = false;

	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;
	VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkSampler sampler;
	VkDevice logicalDeviceHandle;
};