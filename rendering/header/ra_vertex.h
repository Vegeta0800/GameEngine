
#pragma once
#include <vector>
#include <unordered_map>
#include "math/ra_vector3.h"
#include "math/ra_vector2.h"
#include <vulkan\vulkan_core.h>

struct Vertex
{
	Math::Vec3 position;
	Math::Vec3 normal;
	Math::Vec2 uvCoord;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription vertexInputBindingDescription;
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(Vertex);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return vertexInputBindingDescription;
	}

	bool operator== (const Vertex& other) const
	{
		return position == other.position && normal == other.normal && uvCoord == other.uvCoord;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription(3);

		vertexInputAttributeDescription[0].location = 0;
		vertexInputAttributeDescription[0].binding = 0;
		vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescription[0].offset = offsetof(Vertex, position);

		vertexInputAttributeDescription[1].location = 1;
		vertexInputAttributeDescription[1].binding = 0;
		vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescription[1].offset = offsetof(Vertex, normal);

		vertexInputAttributeDescription[2].location = 2;
		vertexInputAttributeDescription[2].binding = 0;
		vertexInputAttributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescription[2].offset = offsetof(Vertex, uvCoord);

		return vertexInputAttributeDescription;
	}
};

namespace std {

	template <> struct hash<Vertex>
	{
		size_t operator()(const Vertex& k) const
		{
			size_t posX = std::hash<float>()(k.position.x);
			size_t posY = std::hash<float>()(k.position.y);
			size_t posZ = std::hash<float>()(k.position.z);

			size_t colorX = std::hash<float>()(k.normal.x);
			size_t colorY = std::hash<float>()(k.normal.y);
			size_t colorZ = std::hash<float>()(k.normal.z);

			size_t UVX = std::hash<float>()(k.uvCoord.x);
			size_t UVY = std::hash<float>()(k.uvCoord.y);

			size_t h1 = ((posX ^ (posY << 1)) >> 1) ^ posZ;
			size_t h2 = ((colorX ^ (colorY << 1)) >> 1) ^ colorZ;
			size_t h3 = ((UVX ^ (UVY << 1)) >> 1);

			return ((h1 ^ (h2 << 1)) >> 1) ^ h3;
		}
	};
}