
#pragma once
//EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
#include <string>
//INTERNAL INCLUDES
#include "ra_types.h"

class Texture
{
public:
	//Load the texture, only possible if values have been set
	void Load();
	//Upload the texture, only possible if values have been set
	void Upload();
	//Release the texture
	void Release();


	//Return the sampler
	VkSampler GetSampler();
	//Return the image view
	VkImageView GetImageView();

	//Get the name
	std::string GetName();
	//Get path as a string reference
	std::string& GetPath();

	//Return the pixels
	byte* GetPixels();

	//Return width of texture
	int GetWidth();
	//Return height of texture
	int GetHeight();
	//Return channels of texture
	int GetChannels();
	//Return byte size of the whole texture
	int GetByteSize();

	//Return if the texture has been loaded
	bool isLoaded();

private:
	//Transfer image
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