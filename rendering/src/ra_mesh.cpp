#include "ra_mesh.h"
#include "ra_texture.h"

Mesh::Mesh(std::vector<Texture*> textures)
{
	this->textures = textures;
}

void Mesh::SetIndicesSize(ui32 size)
{
	this->indicesSize = size;
}

std::string Mesh::GetName()
{
	return this->name;
}

VkBuffer& Mesh::GetVertexBuffer()
{
	return this->vertexBuffer;
}

VkBuffer& Mesh::GetIndexBuffer()
{
	return this->indexBuffer;
}

VkBuffer& Mesh::GetUniformBuffer()
{
	return this->uniformBuffer;
}

VkDeviceMemory& Mesh::GetVertexBufferMem()
{
	return this->vertexBufferMemory;
}

VkDeviceMemory& Mesh::GetUniformBufferMem()
{
	return this->uniformBufferMemory;
}

VkDeviceMemory& Mesh::GetIndexBufferMem()
{
	return this->indexBufferMemory;
}

void Mesh::CreateMesh(std::string name)
{
	this->name = name;
}

std::vector<Texture*> Mesh::GetTextures()
{
	return this->textures;
}

ui32 Mesh::GetIndicesSize()
{
	return this->indicesSize;
}

bool& Mesh::GetInitStatus()
{
	return this->initialized;
}
void Mesh::Cleanup()
{
	this->vertexBuffer = VK_NULL_HANDLE;
	this->indexBuffer = VK_NULL_HANDLE;
	this->vertexBufferMemory = VK_NULL_HANDLE;
	this->indexBufferMemory = VK_NULL_HANDLE;
	this->uniformBuffer = VK_NULL_HANDLE;
	this->uniformBufferMemory = VK_NULL_HANDLE;
}