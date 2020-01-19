
#define TINYOBJLOADER_IMPLEMENTATION //for model loading

//EXTERNAL INCLUDES
#include <unordered_map>
//INTERNAL INCLUDES
#include "ra_model.h"
#include "ra_tinyobjloader.h"
#include "math/ra_vector3.h"
#include "math/ra_vector2.h"

//Create a model with a path
void Model::Create(const char* p)
{
	//Vertex attributes, shapes and material setup
	tinyobj::attrib_t vertexAttributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	//Error and name as string
	std::string errorString;
	std::string tempName(p);

	//reduce the path to only the name
	tempName = tempName.substr(tempName.find_last_of("/") + 1);

	//Set name and path
	this->name = tempName;
	this->path = p;

	//Load the object with the references being filled by the function
	bool meshLoaded = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, nullptr, &errorString, p);

	//If it failed throw.
	if (!meshLoaded)
		throw errorString;

	//Map vertices with vertices size of iteration (amount of indices)
	std::unordered_map<Vertex, ui32> tempVertices;

	//For all shapes (polygons) in the obj
	for(tinyobj::shape_t shape : shapes)
	{
		//For each index in indices of that shape
		for (tinyobj::index_t index : shape.mesh.indices)
		{
			//Get position by using the indices to read the vertices
			Math::Vec3 position = 
			{
				vertexAttributes.vertices[3 * index.vertex_index + 0], 
				vertexAttributes.vertices[3 * index.vertex_index + 2],
				vertexAttributes.vertices[3 * index.vertex_index + 1] 
			};

			//Get texture coordinats by using the indices to read the vertices
			Math::Vec2 texCoords =
			{
				vertexAttributes.texcoords[2 * index.texcoord_index + 0],
				vertexAttributes.texcoords[2 * index.texcoord_index + 1]
			};

			//Get normals by using the indices to read the vertices
			Math::Vec3 normals =
			{
				vertexAttributes.normals[3 * index.normal_index + 0],
				vertexAttributes.normals[3 * index.normal_index + 2],
				vertexAttributes.normals[3 * index.normal_index + 1]
			};

			//Create Vertex object
			Vertex vertex =
			{
				position,
				normals,
				texCoords
			};

			//If the vertex isnt mapped yet
			if (tempVertices.count(vertex) == 0)
			{
				//Map it with the size of the current vertices size (amount of indices)
				tempVertices[vertex] = static_cast<ui32>(tempVertices.size());
				//Push back vertex to vertices
				this->vertices.push_back(vertex);
			}
			//Push back the indices
			this->indices.push_back(tempVertices[vertex]);
		}
	}
}


//Set the path of model
void Model::SetPath(const char* p)
{
	this->path = p;
}
//Set name of the model
void Model::SetName(const char* p)
{
	this->name = p;
}


//Get the vertices
std::vector<Vertex> Model::GetVertices()
{
	return this->vertices;
}
//Get the indices
std::vector<ui32> Model::GetIndices()
{
	return this->indices;
}

//Get the name of the model
std::string Model::GetName()
{
	return this->name;
}