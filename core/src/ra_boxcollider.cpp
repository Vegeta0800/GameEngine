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

std::vector<Math::Vec3>& BoxCollider::GetMinMax()
{
	fColorRGBA projection = fColorRGBA{ this->minMax[0].x, this->minMax[0].y, this->minMax[0].z } *
	(SceneManager::GetInstancePtr()->GetActiveCamera()->GetVPMatrix() *
	Math::CreateModelMatrix(gb->GetTransform().position, gb->GetTransform().scaling, Math::Vec3::zero));

	this->minMax[0] = Math::Vec3
	{
		this->gb->GetTransform().position.x + projection.r,
		this->gb->GetTransform().position.y + projection.g,
		this->gb->GetTransform().position.z + projection.b,
	};

	fColorRGBA projection2 = fColorRGBA{ this->minMax[1].x, this->minMax[1].y, this->minMax[1].z } *
		(SceneManager::GetInstancePtr()->GetActiveCamera()->GetVPMatrix() *
			Math::CreateModelMatrix(gb->GetTransform().position, gb->GetTransform().scaling, Math::Vec3::zero));

	this->minMax[1] = Math::Vec3
	{
		this->gb->GetTransform().position.x + projection2.r,
		this->gb->GetTransform().position.y + projection2.g,
		this->gb->GetTransform().position.z + projection2.b,
	};

	return this->minMax;
}
