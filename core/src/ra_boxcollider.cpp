#include "ra_boxcollider.h"
#include "ra_gameobject.h"
#include "math/ra_mathfunctions.h"
#include "ra_scenemanager.h"
#include "ra_camera.h"

BoxCollider::BoxCollider(Gameobject* gb)
{
	this->minMax.resize(2);
	this->gb = gb;
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
			Math::CreateModelMatrix(((this->gb->GetTransform().position) * 0.1f), gb->GetTransform().scaling, Math::Vec3::zero));

	Math::Vec3 min = Math::Vec3
	{
		this->gb->GetTransform().position.x + projection.r,
		this->gb->GetTransform().position.y + projection.g,
		this->gb->GetTransform().position.z + projection.b,
	};

	return min;
}

Math::Vec3 BoxCollider::GetMax()
{
	fColorRGBA projection = fColorRGBA{ this->minMax[1].x, this->minMax[1].y , this->minMax[1].z, 1.0f } *
		(SceneManager::GetInstancePtr()->GetActiveCamera()->GetVPMatrix() *
			Math::CreateModelMatrix(((this->gb->GetTransform().position) * 0.1f), gb->GetTransform().scaling, Math::Vec3::zero));

	Math::Vec3 max = Math::Vec3
	{
		this->gb->GetTransform().position.x + projection.r,
		this->gb->GetTransform().position.y + projection.g,
		this->gb->GetTransform().position.z + projection.b,
	};

	return max;
}

