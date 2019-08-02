
#pragma once 
#include <vector>

#include "math/ra_vector3.h"
#include "math/ra_mat4x4.h"

class Gameobject;

class BoxCollider
{
public:
	BoxCollider(Gameobject* gb);

	std::vector<Math::Vec3>& GetMinMax();
private:
	std::vector<Math::Vec3> minMax;
	Gameobject* gb;
};