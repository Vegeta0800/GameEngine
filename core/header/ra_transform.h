#pragma once

//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "math/ra_vector3.h"
#include "math/ra_quaternion.h"

class Transform
{
public:
	Math::Vec3 position;
	Math::Vec3 eulerRotation;
	Math::Quaternion rotation;
	Math::Vec3 scaling;
};