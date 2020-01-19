
#pragma once
#include "ra_vertex.h"

class Model
{
public:
	//Create a model with a path
	void Create(const char* p);


	//Set the path of model
	void SetPath(const char* p);
	//Set name of the model
	void SetName(const char* p);


	//Get the vertices
	std::vector<Vertex> GetVertices();
	//Get the indices
	std::vector<ui32> GetIndices();

	//Get the name of the model
	std::string GetName();

private:
	std::vector<Vertex> vertices;
	std::vector<ui32> indices;

	std::string name;

	const char* path;
};