#include "ra_boxcollider.h"
#include "ra_gameobject.h"
#include "math/ra_mathfunctions.h"
#include "ra_scenemanager.h"
#include "ra_camera.h"

BoxCollider::BoxCollider(Transform* transform)
{
	this->minMax.resize(2);
	this->transform = transform;
}

void BoxCollider::SetMinMax(Math::Vec3 min, Math::Vec3 max)
{
	this->minMax[0] = min;
	this->minMax[1] = max;
}

Math::Vec3 BoxCollider::GetMin()
{
	fColorRGBA projection = fColorRGBA{ this->minMax[0].x, this->minMax[0].y , this->minMax[0].z, 1.0f } *
		(SceneManager::GetInstancePtr()->GetActiveCamera()->GetVPMatrix() *
			Math::CreateModelMatrix(((this->transform->position) * 0.1f), 
				this->transform->scaling, Math::Vec3::zero));

	Math::Vec3 min = Math::Vec3
	{
		this->transform->position.x + projection.r,
		this->transform->position.y + projection.g,
		this->transform->position.z + projection.b,
	};

	return min;
}

Math::Vec3 BoxCollider::GetMax()
{
	fColorRGBA projection = fColorRGBA{ this->minMax[1].x, this->minMax[1].y , this->minMax[1].z, 1.0f } *
		(SceneManager::GetInstancePtr()->GetActiveCamera()->GetVPMatrix() *
			Math::CreateModelMatrix(((this->transform->position) * 0.1f), 
				this->transform->scaling, Math::Vec3::zero));

	Math::Vec3 max = Math::Vec3
	{
		this->transform->position.x + projection.r,
		this->transform->position.y + projection.g,
		this->transform->position.z + projection.b,
	};

	return max;
}

std::vector<Math::Vec3> BoxCollider::GetMinMax()
{
	return this->minMax;
}

float& BoxCollider::GetWidth()
{
	return this->width;
}

float& BoxCollider::GetHeight()
{
	return this->height;
}

bool& BoxCollider::GetIsColliding()
{
	return this->isColliding;
}

bool& BoxCollider::hasCollision()
{
	return this->activeCollision;
}

bool& BoxCollider::isTrigger()
{
	return this->trigger;
}