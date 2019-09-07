
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "physic/ra_rigidbody.h"
#include "physic/ra_collision.h"
#include "ra_application.h"
#include "ra_gameobject.h"

Rigidbody::Rigidbody(Transform* transform)
{
	//this->collision = new Collision(gb);
	this->transform = transform;
	this->rigidBody.velocity = 0.0f;
	this->rigidBody.movementDir = Math::Vec3::zero;
}

void Rigidbody::Update()
{
	//if(this->rigidBody.hasCollision)
	//	this->collision->Update();

	if (this->rigidBody.velocity != 0.0f)
	{
		AddForce(this->rigidBody.movementDir, this->rigidBody.velocity);
	}

	//Gravity
	if (this->rigidBody.isEnabled && !this->rigidBody.isKinematic && this->rigidBody.gravityEnabled)
	{
		if (this->rigidBody.isColliding == false)
		{
			Gravity();
		}
	}
}

void Rigidbody::Cleanup(void)
{
}

void Rigidbody::AddForce(Math::Vec3 direction, float force)
{
	float v = CalculateVelocity((force / this->rigidBody.mass));
	this->transform->position += ((direction * v) * Application::GetInstancePtr()->GetDeltaTime());
}

//Math::Vec3 Rigidbody::GetImpactDirection()
//{
//	return this->collision->GetImpactDirection();
//}
//
//Math::Vec3 Rigidbody::GetImpactNormal()
//{
//	return this->collision->GetImpactNormal();
//}

bool Rigidbody::RayCast(Math::Vec3 start, Math::Vec3 end)
{

	return false;
}

bool Rigidbody::RayCast(Math::Vec3 start, Math::Vec3 end, RaycastInfo & hitInfo)
{
	return false;
}

RigidbodyValues& Rigidbody::GetRigidbodyValues()
{
	return this->rigidBody;
}

void Rigidbody::Gravity()
{
	this->rigidBody.gravityDir =  Math::Negate(this->transform->position - gravityCenter);

	this->gravity = this->gravityConstant * ((this->rigidBody.mass * this->massOfCenter) 
		/ (Math::Distance(this->transform->position, this->gravityCenter)) 
		* Math::Distance(this->transform->position, this->gravityCenter));

	this->transform->position += (this->rigidBody.gravityDir * CalculateVelocity(gravity) 
		* Application::GetInstancePtr()->GetDeltaTime());
}

float Rigidbody::CalculateVelocity(float force)
{
	return 
		sqrt((2.0f * abs(force)) / 
		(this->rigidBody.airDensity * this->rigidBody.dragCoefficient * 100.0f));
}
