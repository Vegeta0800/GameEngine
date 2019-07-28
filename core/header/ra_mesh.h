
#pragma once
//EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
//INTERNAL INCLUDES
#include "ra_types.h"

class Mesh
{
public:
	Mesh();

	void SetIndicesSize(ui32 size);

	const char* GetName();

	VkBuffer& GetVertexBuffer();
	VkBuffer& GetIndexBuffer();
	VkBuffer& GetUniformBuffer();

	VkDeviceMemory& GetVertexBufferMem();
	VkDeviceMemory& GetIndexBufferMem();
	VkDeviceMemory& GetUniformBufferMem();

	ui32 GetIndicesSize();

	void CreateMesh(const char* name);
	void Cleanup();
private:

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkBuffer uniformBuffer;

	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory uniformBufferMemory;

	ui32 indicesSize = 0;

	const char* name;
};