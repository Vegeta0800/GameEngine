
#pragma once
#include <vector>

#include "math/ra_vector3.h"
#include "math/ra_mat4x4.h"

class Gameobject;

class Camera
{
public:
	void Initialize();

	void Update();
	void SetTarget(Gameobject* target);

	float& GetOffset();
	Math::Vec3& GetPostion();
	Math::Vec3& GetRotation();
	Math::Vec3& GetDirection();
	Math::Vec3& GetOrientation();
	Math::Vec3& GetTargetOffset();
	Gameobject* GetTarget();
	Math::Mat4x4 GetVPMatrix();
	Math::Mat4x4 GetViewMatrix();
	Math::Mat4x4 GetProjectionMatrix();
	
	bool FrustumCulling(Math::Vec3 min, Math::Vec3 max, std::vector<Vec4> frustum_planes);
private:
	void UpdatePosition();

	Math::Vec3 position;
	Math::Vec3 rotation;
	Math::Vec3 direction;
	Math::Vec3 orientation;
	Math::Vec3 targetOffset;
	Gameobject* target;

	float offset;

	Math::Mat4x4 viewMatrix;
	Math::Mat4x4 projectionMatrix;
};