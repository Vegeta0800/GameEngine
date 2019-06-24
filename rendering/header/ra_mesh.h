
#pragma once
#include "ra_vertex.h"

class Mesh
{
public:
	Mesh();
	void Create(const char* path);

	std::vector<Vertex> GetVertices();
	std::vector<ui32> GetIndices();

private:
	std::vector<Vertex> vertices;
	std::vector<ui32> indices;
};