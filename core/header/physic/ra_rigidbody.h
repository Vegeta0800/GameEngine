
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_types.h"
#include "ra_utils.h"
#include "math/ra_vector3.h"
#include "components/ra_components.h"

class Transform;

//Class RigidBody, holds all physics related code
class Rigidbody
{
	//Declare the public functions that are overwritten.
public:
	//RigidBody constructor setting a lot of standard values
	Rigidbody(Transform* transform, std::string name);

	//Update physics
	void Update(void);
	//Cleanup rigidbody and delete it
	void Cleanup(void);
	//Apply force to a specific direction vector
	void AddForce(Math::Vec3 direction, float force);


	//Set object the rigidbody is colliding with
	void SetHitObject(Rigidbody* object);


	//Get object the rigidbody is colliding with
	Rigidbody* GetHitObject();
	//Get Transform
	Transform* GetTransform();


	//Get rigidbodys values
	RigidbodyValues& GetRigidbodyValues();
	//Get the position of the gravity center
	Math::Vec3& GetGravityCenter();


	//Get name
	std::string GetName();

private:
	//Update the gravity
	void Gravity(void);
	//Calculate the velocity by dividng the force by the rigidbodys mass
	float CalculateVelocity(float force);

	RigidbodyValues rigidBody;
	std::string name;

	//Gravity
	Math::Vec3 gravityCenter;
	const float gravityConstant = 0.000000000006674f; // Gravity constant 6.674 * 10 ^ -11
	float gravity;
	float massOfCenter;

	Transform* transform;
	Rigidbody* hitObject; //Collided Object
};