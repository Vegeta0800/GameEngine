
#pragma once
#include "ra_vertex.h"

class Model
{
public:
	Model(const char* p = "");
	void Create(const char* p);

	void SetPath(const char* p);
	void SetName(const char* p);

	std::vector<Vertex> GetVertices();
	std::vector<ui32> GetIndices();
	const char* GetName();

private:
	std::vector<Vertex> vertices;
	std::vector<ui32> indices;

	const char* path;
	const char* name;
};