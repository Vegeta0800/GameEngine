
#pragma once
#include <vector>
#include "ra_types.h"
#include "ra_vertex.h"

struct Mesh
{
	const char* objectFile;
	ui32 indicesCount;

	std::vector<Vertex> vertices;
	std::vector<ui32> indices;
};
