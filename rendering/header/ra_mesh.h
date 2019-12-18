
#pragma once
//EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
//INTERNAL INCLUDES
#include "ra_types.h"

class Texture;

class Mesh
{
public:
	Mesh(std::vector<Texture*> textures);

	void SetIndicesSize(ui32 size);

	std::string GetName();

	VkBuffer& GetVertexBuffer();
	VkBuffer& GetIndexBuffer();
	VkBuffer& GetUniformBuffer();

	VkDeviceMemory& GetVertexBufferMem();
	VkDeviceMemory& GetIndexBufferMem();
	VkDeviceMemory& GetUniformBufferMem();

	std::vector<Texture*> GetTextures();

	ui32 GetIndicesSize();
	bool& GetInitStatus();

	void CreateMesh(std::string name);
	void Cleanup();
private:

	std::vector<Texture*> textures;

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkBuffer uniformBuffer;

	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory uniformBufferMemory;

	ui32 indicesSize = 0;

	std::string name;

	bool initialized = false;
};