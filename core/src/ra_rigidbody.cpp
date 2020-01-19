
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "physic/ra_rigidbody.h"
#include "ra_application.h"
#include "ra_gameobject.h"

//RigidBody constructor setting a lot of standard values
Rigidbody::Rigidbody(Transform* transform, std::string name)
{
	//Transform
	this->transform = transform;

	//Physics based values
	this->rigidBody.movementDir = Math::Vec3::neg_unit_y;
	this->rigidBody.mass = 1.0f;
	this->rigidBody.isEnabled = true;
	this->rigidBody.force = 0.0f;
	this->rigidBody.isKinematic = false;
	this->rigidBody.gravityEnabled = false;
	this->rigidBody.hasCollision = false;
	this->rigidBody.isColliding = false;
	this->rigidBody.airDensity = 1.20f;
	this->rigidBody.dragCoefficient = 0.024f;
	this->rigidBody.velocity = 0.0f;

	//Name
	this->name = name;

	//Mass of the gravity center
	this->massOfCenter = 12004000000.0f;
}

//Update physics
void Rigidbody::Update()
{
	//If rigidBody is enabled
	if (this->rigidBody.isEnabled)
	{
		//If there is force that should be applied
		if (this->rigidBody.force != 0.0f)
		{
			//Add that force to the current movement direction
			AddForce(this->rigidBody.movementDir, this->rigidBody.force);
		}

		//If gravity is enabled and rigidbody isnt kinematic
		if (!this->rigidBody.isKinematic && this->rigidBody.gravityEnabled)
		{
			//If rigidbody isnt colliding
			if (this->rigidBody.isColliding == false)
			{
				//Update gravity
				Gravity();
			}
		}
	}
}
//Cleanup rigidbody and delete it
void Rigidbody::Cleanup(void)
{
	this->transform = nullptr;
	this->hitObject = nullptr;
	delete this;
}
//Apply force to a specific direction vector
void Rigidbody::AddForce(Math::Vec3 direction, float force)
{
	//Calculate the forces veloctiy
	float v = CalculateVelocity((force));

	//Update the transforms position
	this->transform->position += ((direction * v) * Application::GetInstancePtr()->GetDeltaTime());
}


//Set object the rigidbody is colliding with
void Rigidbody::SetHitObject(Rigidbody* object)
{
	this->hitObject = object;
}


//Get object the rigidbody is colliding with
Rigidbody* Rigidbody::GetHitObject()
{
	return this->hitObject;
}
//Get Transform
Transform* Rigidbody::GetTransform()
{
	return this->transform;
}

//Get rigidbodys values
RigidbodyValues& Rigidbody::GetRigidbodyValues()
{
	return this->rigidBody;
}
//Get the position of the gravity center
Math::Vec3& Rigidbody::GetGravityCenter()
{
	return this->gravityCenter;
}

//Get name
std::string Rigidbody::GetName()
{
	return this->name;
}


//Update the gravity
void Rigidbody::Gravity()
{
	//Get the gravity direction
	this->rigidBody.gravityDir =  Math::Negate(this->transform->position - gravityCenter);

	//Calculate the gravity force
	this->gravity = this->gravityConstant * ((this->rigidBody.mass * this->massOfCenter) 
		/ (Math::Distance(this->transform->position, this->gravityCenter)) 
		* Math::Distance(this->transform->position, this->gravityCenter));

	//Update transforms positions with gravitys force and direction.
	this->transform->position += (this->rigidBody.gravityDir * CalculateVelocity(gravity) 
		* Application::GetInstancePtr()->GetDeltaTime());
}
float Rigidbody::CalculateVelocity(float force)
{
	return force / this->rigidBody.mass;
}