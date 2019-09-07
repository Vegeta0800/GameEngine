//
////EXTERNAL INCLUDES
////INTERNAL INCLUDES
//#include "ra_scenemanager.h"
//#include "ra_gameobject.h"
//#include "input/ra_inputhandler.h"
//#include "physic/ra_collision.h"
//#include "physic/ra_rigidbody.h"
//#include "ra_utils.h"
//#include "ra_camera.h"
//#include "ra_rendering.h"
//#include "math/ra_vector3.h"
//#include "math/ra_mathfunctions.h"
//
//
//Collision::Collision(Gameobject* gb)
//{
//	this->gameobject = gb;
//}
//
//void Collision::Update(void)
//{
//	//Execute the components Update function.
//
//	if (Input::GetInstancePtr()->GetKey(KeyCode::B))
//	{
//		float f = 0.01f;
//	}
//
//	if (this->gameobject->hasCollision())
//	{
//		for (Gameobject* gb : SceneManager::GetInstancePtr()->GetActiveScene()->GetAllChildren())
//		{
//			if (gb->hasCollision())
//			{
//				if (gb != this->gameobject)
//				{
//					if (Math::Distance(this->gameobject->GetTransform().position, gb->GetTransform().position) < GetRadius(gb))
//					{
//						if (CheckCollision(gb) == true)
//						{
//							if (this->gameobject->GetHitObject() == nullptr)
//							{
//								this->impactDir = Math::Negate(this->gameobject->GetRigidbody()->GetRigidbodyValues().movementDir);
//								this->gameobject->SetHitObject(gb);
//								this->impactNormal = CalculateImpactNormal();
//							}
//
//							this->gameobject->hasCollision() = true;
//						}
//						else
//						{
//							if (this->gameobject->GetHitObject() == gb)
//							{
//								this->gameobject->SetHitObject(nullptr);
//								this->gameobject->hasCollision() = false;
//							}
//						}
//					}
//					else
//					{
//						if (this->gameobject->GetHitObject() == gb)
//						{
//							this->gameobject->SetHitObject(nullptr);
//							this->gameobject->hasCollision() = false;
//						}
//					}
//				}
//			}
//			else
//			{
//				if (this->gameobject->GetHitObject() == gb)
//				{
//					this->gameobject->SetHitObject(nullptr);
//					this->gameobject->hasCollision() = false;
//				}
//			}
//		}
//	}
//}
//
//void Collision::Cleanup(void)
//{
//
//}
//
//Math::Vec3 Collision::GetImpactDirection()
//{
//	return this->impactDir;
//}
//
//Math::Vec3 Collision::GetImpactNormal()
//{
//	return this->impactNormal;
//}
//
//Math::Vec3 Collision::CalculateImpactNormal()
//{
//	if
//	(
//   			this->gameobject->GetHitObject()->GetBoxCollider()->GetMax().x > this->gameobject->GetTransform().position.x &&
//			this->gameobject->GetHitObject()->GetBoxCollider()->GetMin().x < this->gameobject->GetTransform().position.x
//	)
//	{
//		if (this->gameobject->GetHitObject()->GetTransform().position.y > this->gameobject->GetTransform().position.y)
//			return Math::Vec3::neg_unit_y;
//		else
//			return Math::Vec3::unit_y;
//	}
//	else
//	{
//		if (this->gameobject->GetHitObject()->GetTransform().position.x > this->gameobject->GetTransform().position.x)
//			return Math::Vec3::unit_x;
//		else
//			return Math::Vec3::neg_unit_x;
//	}
//}
//
//float Collision::GetRadius(Gameobject* gb)
//{
//	return Math::Length((gb->GetBoxCollider()->GetMax() - gb->GetBoxCollider()->GetMin()));
//}
//
//Math::Vec3 Collision::GetAxis(Math::Vec3 point1, Math::Vec3 point2)
//{
//	Math::Vec3 edge = point1 - point2;
//	Math::Vec3 edgeNormal = Math::Vec3{ -edge.y, edge.x, edge.z };
//	Math::Normalize(edgeNormal);
//	return edgeNormal;
//}
//
//bool Collision::isProjectionIntersecting(Math::Vec3 aCorners[], Math::Vec3 bCorners[], Math::Vec3 axis)
//{
//	float aMin, aMax, bMin, bMax;
//
//	Math::GetMinMaxOfProjection(aCorners, axis, aMin, aMax);
//	Math::GetMinMaxOfProjection(bCorners, axis, bMin, bMax);
//
//	if (aMin > bMax) return false;
//	if (aMax < bMin) return false;
//
//	return true;
//}
//
//bool Collision::CheckCollision(Gameobject* gb)
//{
//	Math::Mat4x4 aTemp = SceneManager::GetInstancePtr()->GetActiveCamera()->GetVPMatrix() *
//		Math::CreateModelMatrix((this->gameobject->GetTransform().position * 0.1), this->gameobject->GetTransform().scaling, Math::Negate(this->gameobject->GetTransform().eulerRotation));
//
//	Math::Mat4x4 bTemp = SceneManager::GetInstancePtr()->GetActiveCamera()->GetVPMatrix() *
//		Math::CreateModelMatrix((gb->GetTransform().position * 0.1), gb->GetTransform().scaling, Math::Negate(gb->GetTransform().eulerRotation));
//
//	Vec4 aCorners4[4] = 
//	{
//		Vec4{this->gameobject->GetBoxCollider()->GetMax().x , this->gameobject->GetBoxCollider()->GetMax().y, 0.0f, 1.0f} * aTemp,
//		Vec4{this->gameobject->GetBoxCollider()->GetMin().x , this->gameobject->GetBoxCollider()->GetMax().y, 0.0f, 1.0f} * aTemp,
//		Vec4{this->gameobject->GetBoxCollider()->GetMin().x , this->gameobject->GetBoxCollider()->GetMin().y, 0.0f, 1.0f} * aTemp,
//		Vec4{this->gameobject->GetBoxCollider()->GetMax().x , this->gameobject->GetBoxCollider()->GetMin().y, 0.0f, 1.0f} * aTemp,
//	};
//
//	Vec4 bCorners4[4] =
//	{
//		Vec4{gb->GetBoxCollider()->GetMax().x , gb->GetBoxCollider()->GetMax().y, 0.0f, 1.0f} * bTemp,
//		Vec4{gb->GetBoxCollider()->GetMin().x , gb->GetBoxCollider()->GetMax().y, 0.0f, 1.0f} * bTemp,
//		Vec4{gb->GetBoxCollider()->GetMin().x , gb->GetBoxCollider()->GetMin().y, 0.0f, 1.0f} * bTemp,
//		Vec4{gb->GetBoxCollider()->GetMax().x , gb->GetBoxCollider()->GetMin().y, 0.0f, 1.0f} * bTemp,
//	};
//
//	Math::Vec3 aCorners[4] =
//	{
//		Math::Vec3{ this->gameobject->GetTransform().position.x + aCorners4[0].r * 10.0f, this->gameobject->GetTransform().position.y + aCorners4[0].g * 10.0f, 0.0f},
//		Math::Vec3{ this->gameobject->GetTransform().position.x + aCorners4[1].r * 10.0f, this->gameobject->GetTransform().position.y + aCorners4[1].g * 10.0f, 0.0f},
//		Math::Vec3{ this->gameobject->GetTransform().position.x + aCorners4[2].r * 10.0f, this->gameobject->GetTransform().position.y + aCorners4[2].g * 10.0f, 0.0f},
//		Math::Vec3{ this->gameobject->GetTransform().position.x + aCorners4[3].r * 10.0f, this->gameobject->GetTransform().position.y + aCorners4[3].g * 10.0f, 0.0f},
//	};
//
//	Math::Vec3 bCorners[4] =
//	{
//		Math::Vec3{ gb->GetTransform().position.x + bCorners4[0].r * 10.0f, gb->GetTransform().position.y + bCorners4[0].g * 10.0f, 0.0f},
//		Math::Vec3{ gb->GetTransform().position.x + bCorners4[1].r * 10.0f, gb->GetTransform().position.y + bCorners4[1].g * 10.0f, 0.0f},
//		Math::Vec3{ gb->GetTransform().position.x + bCorners4[2].r * 10.0f, gb->GetTransform().position.y + bCorners4[2].g * 10.0f, 0.0f},
//		Math::Vec3{ gb->GetTransform().position.x + bCorners4[3].r * 10.0f, gb->GetTransform().position.y + bCorners4[3].g * 10.0f, 0.0f},
//	};
//
//	Math::Vec3 axis1 = GetAxis(aCorners[0], aCorners[1]);
//	if (!this->isProjectionIntersecting(aCorners, bCorners, axis1)) return false;
//
//	Math::Vec3 axis2 = GetAxis(aCorners[0], aCorners[3]);
//	if (!this->isProjectionIntersecting(aCorners, bCorners, axis2)) return false;
//
//	Math::Vec3 axis3 = GetAxis(bCorners[1], bCorners[2]);
//	if (!this->isProjectionIntersecting(aCorners, bCorners, axis3)) return false;
//
//	Math::Vec3 axis4 = GetAxis(bCorners[1], bCorners[0]);
//	if (!this->isProjectionIntersecting(aCorners, bCorners, axis4)) return false;
//
//	return true;
//}
//
//
