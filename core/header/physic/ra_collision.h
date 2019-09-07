
#pragma once

//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "math/ra_vector3.h"
#include "math/ra_vector2.h"
#include "math/ra_mat4x4.h"

class Gameobject;

//Class Movement that is derived from class Component.
class Collision
{
	//Declare the public functions that are overwritten.
public:
	Collision(Gameobject* gb);

	void Update(void);
	void Cleanup(void);
	Math::Vec3 GetImpactDirection();
	Math::Vec3 GetImpactNormal();

private:
	float GetRadius(Gameobject* gb);

	Math::Vec3 CalculateImpactNormal();
	Math::Vec3 GetAxis(Math::Vec3 point1, Math::Vec3 point2);
	bool isProjectionIntersecting(Math::Vec3 aCorners[], Math::Vec3 bCorners[], Math::Vec3 normal);


	bool CheckCollision(Gameobject* gb);

	float collisionRange;
	Math::Vec3 impactDir;
	Math::Vec3 impactNormal;
	Gameobject* gameobject;
};
