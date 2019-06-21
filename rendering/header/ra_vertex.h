
#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "math/ra_vector3.h"

struct Vertex
{
	Math::Vec3 position;
	Math::Vec3 color;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription vertexInputBindingDescription;
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(Vertex);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return vertexInputBindingDescription;

	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription(2);

		vertexInputAttributeDescription[0].location = 0;
		vertexInputAttributeDescription[0].binding = 0;
		vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexInputAttributeDescription[0].offset = offsetof(Vertex, position);

		vertexInputAttributeDescription[1].location = 1;
		vertexInputAttributeDescription[1].binding = 0;
		vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexInputAttributeDescription[1].offset = offsetof(Vertex, color);

		return vertexInputAttributeDescription;
	}
};
