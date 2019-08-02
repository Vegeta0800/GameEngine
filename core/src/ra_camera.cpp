
#include "ra_camera.h"
#include "input/ra_inputhandler.h"
#include "ra_application.h"

void Camera::Initialize()
{
	this->position = Math::Vec3::zero;
	this->rotation = Math::Vec3::zero;
	this->direction = Math::Vec3::unit_y;
	this->orientation = Math::Vec3::unit_z;
	this->offset = 10.0f;

	this->viewMatrix = Math::Mat4x4::identity;
	this->projectionMatrix = Math::Mat4x4::identity;
}

void Camera::Update()
{
	UpdatePosition();
	FrustumCulling();
}

void Camera::FrustumCulling()
{

}

void Camera::UpdatePosition()
{
	float time = Application::GetInstancePtr()->GetDeltaTime();

	if (Input::GetInstancePtr()->GetKey(KeyCode::D))
	{
		this->position.x += 10.0f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::A))
	{
		this->position.x -= 10.0f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::W))
	{
		this->position.z += 10.0f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::S))
	{
		this->position.z -= 10.0f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::Q))
	{
		this->position.y += 10.0f * time;
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::E))
	{
		this->position.y -= 10.0f * time;
	}

	printf("%f, %f. %f \n", this->position.x, this->position.y, this->position.z);

	this->viewMatrix = Math::CreateViewMatrixLookAt(this->position, this->position + (this->direction * this->offset), this->orientation);
	this->projectionMatrix = Math::CreateProjectionMatrix(DegToRad(45.0f), Application::GetInstancePtr()->GetAspectRatio(), 0.1f, 100.0f);
	this->projectionMatrix.m22 *= -1.0f;
}

Math::Vec3& Camera::GetPostion()
{
	return this->position;
}

Math::Vec3& Camera::GetRotation()
{
	return this->rotation;
}

float& Camera::GetOffset()
{
	return this->offset;
}

Math::Vec3& Camera::GetDirection()
{
	return this->direction;
}

Math::Mat4x4 Camera::GetVPMatrix()
{
	return this->projectionMatrix * this->viewMatrix;
}

Math::Mat4x4 Camera::GetViewMatrix()
{
	return this->viewMatrix;
}

Math::Mat4x4 Camera::GetProjectionMatrix()
{
	return this->projectionMatrix;
}
