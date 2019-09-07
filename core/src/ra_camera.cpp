
#include "ra_camera.h"
#include "input/ra_inputhandler.h"
#include "ra_application.h"
#include "ra_gameobject.h"

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

void Camera::Update()
{
	UpdatePosition();
}

bool Camera::FrustumCulling(Math::Vec3 Min, Math::Vec3 Max, std::vector<Vec4> frustum_planes)
{
	bool inside = true;

	//test all 6 frustum planes
	for (int i = 0; i < 6; i++)
	{
		float d = max(Min.x * frustum_planes[i].r, Max.x * frustum_planes[i].r) +
			max(Min.y * frustum_planes[i].g, Max.y * frustum_planes[i].g) +
			max(Min.z * frustum_planes[i].b, Max.z * frustum_planes[i].b) +
			frustum_planes[i].a;

		inside &= d > 0;
	}
	return inside;
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

	if (this->target != nullptr)
		this->viewMatrix = Math::CreateViewMatrixLookAt(this->position, this->target->position + this->targetOffset, this->orientation);
	else
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

Transform* Camera::GetTarget()
{
	return this->target;
}

void Camera::SetTarget(Transform* target)
{
	this->target = target;
}

float& Camera::GetOffset()
{
	return this->offset;
}

Math::Vec3& Camera::GetDirection()
{
	return this->direction;
}

Math::Vec3& Camera::GetTargetOffset()
{
	return this->targetOffset;
}

Math::Vec3& Camera::GetOrientation()
{
	return this->orientation;
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
