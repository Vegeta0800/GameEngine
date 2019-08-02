
#pragma once
#include "math/ra_vector3.h"
#include "math/ra_mat4x4.h"

class Camera
{
public:
	void Initialize();

	void Update();

	float& GetOffset();
	Math::Vec3& GetPostion();
	Math::Vec3& GetRotation();
	Math::Vec3& GetDirection();
	Math::Vec3& GetOrientation();
	Math::Mat4x4 GetVPMatrix();
	Math::Mat4x4 GetViewMatrix();
	Math::Mat4x4 GetProjectionMatrix();
	
private:
	void FrustumCulling();
	void UpdatePosition();

	Math::Vec3 position;
	Math::Vec3 rotation;
	Math::Vec3 direction;
	Math::Vec3 orientation;

	float offset;

	Math::Mat4x4 viewMatrix;
	Math::Mat4x4 projectionMatrix;
};