
#pragma once
#include "ra_vertex.h"

class Model
{
public:
	Model();
	void Create(const char* p);

	void SetPath(const char* p);
	void SetName(const char* p);

	std::vector<Vertex> GetVertices();
	std::vector<ui32> GetIndices();
	std::string GetName();

private:
	std::vector<Vertex> vertices;
	std::vector<ui32> indices;

	std::string name;
	const char* path;
};