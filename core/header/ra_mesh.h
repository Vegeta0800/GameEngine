
#pragma once
//EXTERNAL INCLUDES
#include <vulkan/vulkan.h>
//INTERNAL INCLUDES

class Mesh
{
public:
	VkBuffer GetVertexBuffer();
	VkBuffer GetIndexBuffer();

	VkDeviceMemory GetVertexBufferMem();
	VkDeviceMemory GetIndexBufferMem();
private:
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;

	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory vertexBufferMemory;

};