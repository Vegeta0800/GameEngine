
#pragma once
//EXTERNAL INCLUDES
#include <vector>
//INTERNAL INCLUDES
#include "math/ra_vector3.h"
#include "math/ra_mat4x4.h"

class Transform;

//Camera class
class Camera
{
public:
	//Initialize Camera with standard values
	void Initialize();
	//Update each frame
	void Update();


	//Set cameras target transform
	void SetTarget(Transform* target);


	//Get cameras target transform
	Transform* GetTarget();
	
	//Get cameras position
	Math::Vec3& GetPostion();
	//Get cameras rotation
	Math::Vec3& GetRotation();
	//Get reference to the direction
	Math::Vec3& GetDirection();
	//Get reference to the target offset
	Math::Vec3& GetTargetOffset();
	//Get reference to the orientation
	Math::Vec3& GetOrientation();
	
	//Get view projection matrix
	Math::Mat4x4 GetVPMatrix();
	//Get view matrix
	Math::Mat4x4 GetViewMatrix();
	//Get projection matrix
	Math::Mat4x4 GetProjectionMatrix();
	
	//Get reference to the offset
	float& GetOffset();

private:
	//Update the cameras position
	void UpdatePosition();

	Math::Vec3 position;
	Math::Vec3 rotation;
	Math::Vec3 direction;
	Math::Vec3 orientation;
	Math::Vec3 targetOffset;
	Transform* target;

	float offset;

	Math::Mat4x4 viewMatrix;
	Math::Mat4x4 projectionMatrix;
};