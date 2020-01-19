
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
	//Construct Mesh... can only be constructed using textures
	Mesh(std::vector<Texture*> textures);

	//Name the mesh
	void NameMesh(std::string name);
	//Cleanup the mesh by setting all handles to null
	void Cleanup();


	//Set the indices size 
	void SetIndicesSize(ui32 size);


	//Get the name of the mesh
	std::string GetName();

	//Get the meshes textures
	std::vector<Texture*> GetTextures();

	//Get the vertex buffer reference of the mesh
	VkBuffer& GetVertexBuffer();
	//Get the index buffer reference of the mesh
	VkBuffer& GetIndexBuffer();
	//Get the uniform buffer reference of the mesh
	VkBuffer& GetUniformBuffer();

	//Get the vertex buffer memory reference of the mesh
	VkDeviceMemory& GetVertexBufferMem();
	//Get the index buffer memory reference of the mesh
	VkDeviceMemory& GetIndexBufferMem();
	//Get the uniform buffer memory reference of the mesh
	VkDeviceMemory& GetUniformBufferMem();

	//Get indices size
	ui32 GetIndicesSize();

	//Get initialized state of mesh
	bool& GetInitStatus();

private:

	std::vector<Texture*> textures;

	std::string name;

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkBuffer uniformBuffer;

	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory uniformBufferMemory;

	ui32 indicesSize = 0;

	bool initialized = false;
};