
#define TINYOBJLOADER_IMPLEMENTATION
#include <unordered_map>

#include "ra_mesh.h"
#include "ra_tinyobjloader.h"
#include "math/ra_vector3.h"
#include "math/ra_vector2.h"

Mesh::Mesh()
{
}

void Mesh::Create(const char * path)
{
	tinyobj::attrib_t vertexAttributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errorString;
	std::string warnString;

	bool meshLoaded = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &warnString, &errorString, path);

	if (!meshLoaded)
		throw errorString;

	std::unordered_map<Vertex, ui32> tempVertices;

	for(tinyobj::shape_t shape : shapes)
	{
		for (tinyobj::index_t index : shape.mesh.indices)
		{
			Math::Vec3 position = 
			{
				vertexAttributes.vertices[3 * index.vertex_index + 0], 
				vertexAttributes.vertices[3 * index.vertex_index + 2],
				vertexAttributes.vertices[3 * index.vertex_index + 1] 
			};

			Math::Vec2 texCoords =
			{
				vertexAttributes.texcoords[2 * index.texcoord_index + 0],
				vertexAttributes.texcoords[2 * index.texcoord_index + 1]
			};

			Vertex vertex =
			{
				position,
				Math::Vec3{0.874f, 0.749f, 0.164f},
				texCoords
			};

			if (tempVertices.count(vertex) == 0)
			{
				tempVertices[vertex] = static_cast<ui32>(tempVertices.size());
				this->vertices.push_back(vertex);
			}
			this->indices.push_back(tempVertices[vertex]);
		}
	}

}

std::vector<Vertex> Mesh::GetVertices()
{
	return this->vertices;
}

std::vector<ui32> Mesh::GetIndices()
{
	return this->indices;
}
