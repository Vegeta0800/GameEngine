#include "ra_mesh.h"

Mesh::Mesh()
{
}

void Mesh::SetIndicesSize(ui32 size)
{
	this->indicesSize = size;
}

const char* Mesh::GetName()
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

void Mesh::CreateMesh(const char* name)
{
	this->name = name;
}

ui32 Mesh::GetIndicesSize()
{
	return this->indicesSize;
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