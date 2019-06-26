#pragma once
#include "ra_types.h"
#include "math/ra_vector3.h"

struct Material
{
	Math::Vec3 fragColor;
	Math::Vec3 specularColor;
	float ambientValue;
	float specularValue;
};