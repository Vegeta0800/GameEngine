#pragma once
//EXTERNAL INCLUDES
#include <limits>
//INTERNAL INCLUDES
#include "ra_types.h"
#include "ra_utils.h"
#include "math/ra_vector3.h"
#include "components/ra_components.h"

class Transform;

class Rigidbody
{
	//Declare the public functions that are overwritten.
public:
	Rigidbody(Transform* transform, std::string name);

	void Update(void);
	void Cleanup(void);

	void AddForce(Math::Vec3 direction, float force);

	void SetHitObject(Rigidbody* object);

	Math::Vec3 GetImpactDirection();
	Math::Vec3 GetImpactNormal();

	Math::Vec3& GetGravityCenter();

	bool RayCast(Math::Vec3 start, Math::Vec3 end);
	bool RayCast(Math::Vec3 start, Math::Vec3 end, RaycastInfo& hitInfo);
	RigidbodyValues& GetRigidbodyValues();

	Rigidbody* GetHitObject();

	std::string GetName();

	Transform* GetTransform();

private:
	void Gravity(void);
	float CalculateVelocity(float force);

	RigidbodyValues rigidBody;

	std::string name;

	//Gravity
	const float gravityConstant = 0.000000000006674f; // Gravity constant 6.674 * 10 ^ -11
	float gravity;
	float massOfCenter;
	Math::Vec3 gravityCenter;
	Transform* transform;

	Rigidbody* hitObject;
};