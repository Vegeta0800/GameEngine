
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_camera.h"
#include "input/ra_inputhandler.h"
#include "ra_application.h"
#include "ra_gameobject.h"

//Initialize Camera with standard values
void Camera::Initialize()
{
	this->position = Math::Vec3::zero;
	this->rotation = Math::Vec3::zero;
	this->direction = Math::Vec3::unit_y;
	this->orientation = Math::Vec3::unit_z;
	this->targetOffset = Math::Vec3::zero;
	this->target = nullptr;
	this->offset = 1.0f;

	this->viewMatrix = Math::Mat4x4::identity;
	this->projectionMatrix = Math::Mat4x4::identity;
}
//Update each frame
void Camera::Update()
{
	UpdatePosition();
}


//Set cameras target transform
void Camera::SetTarget(Transform* target)
{
	this->target = target;
}


//Get cameras target transform
Transform* Camera::GetTarget()
{
	return this->target;
}


//Get cameras position
Math::Vec3& Camera::GetPostion()
{
	return this->position;
}
//Get cameras rotation
Math::Vec3& Camera::GetRotation()
{
	return this->rotation;
}
//Get reference to the direction
Math::Vec3& Camera::GetDirection()
{
	return this->direction;
}
//Get reference to the target offset
Math::Vec3& Camera::GetTargetOffset()
{
	return this->targetOffset;
}
//Get reference to the orientation
Math::Vec3& Camera::GetOrientation()
{
	return this->orientation;
}

//Get view projection matrix
Math::Mat4x4 Camera::GetVPMatrix()
{
	return this->projectionMatrix * this->viewMatrix;
}
//Get view matrix
Math::Mat4x4 Camera::GetViewMatrix()
{
	return this->viewMatrix;
}
//Get projection matrix
Math::Mat4x4 Camera::GetProjectionMatrix()
{
	return this->projectionMatrix;
}

//Get reference to the offset
float& Camera::GetOffset()
{
	return this->offset;
}


//Update the cameras position
void Camera::UpdatePosition()
{
	//If camera is locked on a target
	if (this->target != nullptr)
	{
		//Create a view matrix that is looking at the target
		this->viewMatrix = Math::CreateViewMatrixLookAt(this->position * 0.1f, this->target->position, this->orientation);
	}
	//If camera isnt locked on a target
	else
		//Look at the cameras position plus its offset and create the view matrix
		this->viewMatrix = Math::CreateViewMatrixLookAt(this->position * 0.1f, this->position * 0.1f + (this->direction * this->targetOffset), this->orientation);

	//Create projection matrix  
	this->projectionMatrix = Math::CreateProjectionMatrix(DegToRad(45.0f), Application::GetInstancePtr()->GetAspectRatio(), 0.1f, 100.0f);
	this->projectionMatrix.m22 *= -1.0f; //Inverse for vulkan
}