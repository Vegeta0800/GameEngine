
#pragma once
#include "ra_vertex.h"

class Mesh
{
public:
	Mesh(const char* p = "");
	void Create(const char* p);

	void SetPath(const char* p);

	std::vector<Vertex> GetVertices();
	std::vector<ui32> GetIndices();

private:
	std::vector<Vertex> vertices;
	std::vector<ui32> indices;

	const char* path;
};