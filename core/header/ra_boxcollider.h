
#pragma once 
#include <vector>

#include "math/ra_vector3.h"
#include "math/ra_mat4x4.h"

class Gameobject;

class BoxCollider
{
public:
	BoxCollider(Gameobject* gb);

	void SetMinMax(Math::Vec3 min, Math::Vec3 max);

	Math::Vec3 GetMin();
	Math::Vec3 GetMax();
private:
	std::vector<Math::Vec3> minMax;
	Gameobject* gb;
};