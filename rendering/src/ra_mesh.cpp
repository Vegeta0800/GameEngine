
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_mesh.h"
#include "ra_texture.h"

//Construct Mesh... can only be constructed using textures
Mesh::Mesh(std::vector<Texture*> textures)
{
	//Set textures
	this->textures = textures;
}

//Name the mesh
void Mesh::NameMesh(std::string name)
{
	this->name = name;
}
//Cleanup the mesh by setting all handles to null
void Mesh::Cleanup()
{
	this->vertexBuffer = VK_NULL_HANDLE;
	this->indexBuffer = VK_NULL_HANDLE;
	this->vertexBufferMemory = VK_NULL_HANDLE;
	this->indexBufferMemory = VK_NULL_HANDLE;
	this->uniformBuffer = VK_NULL_HANDLE;
	this->uniformBufferMemory = VK_NULL_HANDLE;
}


//Set the indices size 
void Mesh::SetIndicesSize(ui32 size)
{
	this->indicesSize = size;
}


//Get the name of the mesh
std::string Mesh::GetName()
{
	return this->name;
}

//Get the meshes textures
std::vector<Texture*> Mesh::GetTextures()
{
	return this->textures;
}

//Get the vertex buffer reference of the mesh
VkBuffer& Mesh::GetVertexBuffer()
{
	return this->vertexBuffer;
}
//Get the index buffer reference of the mesh
VkBuffer& Mesh::GetIndexBuffer()
{
	return this->indexBuffer;
}
//Get the uniform buffer reference of the mesh
VkBuffer& Mesh::GetUniformBuffer()
{
	return this->uniformBuffer;
}

//Get the vertex buffer memory reference of the mesh
VkDeviceMemory& Mesh::GetVertexBufferMem()
{
	return this->vertexBufferMemory;
}
//Get the index buffer memory reference of the mesh
VkDeviceMemory& Mesh::GetUniformBufferMem()
{
	return this->uniformBufferMemory;
}
//Get the uniform buffer memory reference of the mesh
VkDeviceMemory& Mesh::GetIndexBufferMem()
{
	return this->indexBufferMemory;
}

//Get indices size
ui32 Mesh::GetIndicesSize()
{
	return this->indicesSize;
}

//Get initialized state of mesh
bool& Mesh::GetInitStatus()
{
	return this->initialized;
}