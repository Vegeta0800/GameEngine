#pragma once
#include "ra_types.h"
#include "math/ra_vector3.h"

struct Material
{
	fColorRGBA fragColor;
	fColorRGBA specularColor;
	float ambientValue;
	float specularValue;
};