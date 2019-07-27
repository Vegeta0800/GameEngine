
#define TINYOBJLOADER_IMPLEMENTATION
#include <unordered_map>

#include "ra_model.h"
#include "ra_tinyobjloader.h"
#include "math/ra_vector3.h"
#include "math/ra_vector2.h"

Model::Model(const char* p)
{
	this->path = p;
}

void Model::Create(const char* p)
{
	tinyobj::attrib_t vertexAttributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errorString;

	std::string tempName(p);

	tempName = tempName.substr(tempName.find_last_of("/") + 1);

	this->name = tempName.c_str();

	bool meshLoaded = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, nullptr, &errorString, p);

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

			Math::Vec3 normals =
			{
				vertexAttributes.normals[3 * index.normal_index + 0],
				vertexAttributes.normals[3 * index.normal_index + 2],
				vertexAttributes.normals[3 * index.normal_index + 1]
			};

			Vertex vertex =
			{
				position,
				normals,
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

void Model::SetPath(const char* p)
{
	this->path = p;
}

void Model::SetName(const char* p)
{
	this->name = p;
}

const char* Model::GetName()
{
	return this->name;
}

std::vector<Vertex> Model::GetVertices()
{
	return this->vertices;
}

std::vector<ui32> Model::GetIndices()
{
	return this->indices;
}
