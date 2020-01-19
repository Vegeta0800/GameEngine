
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "math/ra_vector3.h"
#include "ra_types.h"

//Material class holds shader values
class Material
{
public:
	fColorRGBA fragColor;
	float ambientValue;
	float specularValue;
};