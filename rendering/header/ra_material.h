#pragma once
#include "ra_types.h"
#include "math/ra_vector3.h"

class Material
{
public:
	fColorRGBA fragColor;
	float ambientValue;
	float specularValue;
};