#include "components/ra_component.h"
#include "components/player/ra_player.h"
#include "input/ra_inputhandler.h"
#include "physic/ra_rigidbody.h"
#include "ra_application.h"
#include "ra_scenemanager.h"
#include "ra_scene.h"
#include "ra_material.h"
#include "ra_transform.h"
#include "components/player/ra_bullet.h"

Player::Player()
{

}

void Player::Initialize(Rigidbody* rigidBody)
{
	Component::Initialize("Player", ComponentType::Player);
	this->rigidBody = rigidBody;
	this->player.speed = 30.0f;
}

void Player::Update()
{
	Component::Update();

	Move();

	if (Input::GetInstancePtr()->GetKeyUp(KeyCode::Space))
	{
		Shoot();
		Input::GetInstancePtr()->GetMyData().shoot = true;
	}
	else
		Input::GetInstancePtr()->GetMyData().shoot = false;
}

void Player::Cleanup()
{
	this->rigidBody = nullptr;
	Component::Cleanup();
}

PlayerValues& Player::GetPlayerValues(void)
{
	return this->player;
}

void Player::Move()
{
	if (Input::GetInstancePtr()->GetKey(KeyCode::A))
	{
		this->rigidBody->AddForce(Math::Vec3::neg_unit_x, this->player.speed);
		Input::GetInstancePtr()->GetMyData().left = true;
	}
	else
		Input::GetInstancePtr()->GetMyData().left = false;

	if (Input::GetInstancePtr()->GetKey(KeyCode::D))
	{
		this->rigidBody->AddForce(Math::Vec3::unit_x, this->player.speed);
		Input::GetInstancePtr()->GetMyData().right = true;
	}
	else
		Input::GetInstancePtr()->GetMyData().right = false;

	//printf("%f, %f, %f \n", this->rigidBody->GetTransform()->position.x, this->rigidBody->GetTransform()->position.y, this->rigidBody->GetTransform()->position.z);
}

void Player::Shoot()
{
	Scene* temp = SceneManager::GetInstancePtr()->GetActiveScene();
	std::string bullet = temp->GetObjectOfPool("bullets");
	temp->SetActive(bullet, true);
	temp->GetRigidBody(bullet)->GetTransform()->position = this->rigidBody->GetTransform()->position;
	temp->GetRigidBody(bullet)->GetTransform()->scaling.x = 0.5f;
	temp->GetRigidBody(bullet)->GetTransform()->scaling.z = 0.5f;
	temp->GetRigidBody(bullet)->GetTransform()->scaling.y = 0.3f;
	temp->GetMaterial(bullet)->fragColor = fColorRGBA{ 0.117f, 0.474f, 0.862f, 1.0f };
	reinterpret_cast<Bullet*>(temp->GetObjectComponent(bullet))->SetParent(this->rigidBody);
	reinterpret_cast<Bullet*>(temp->GetObjectComponent(bullet))->GetBulletValues().activated = true;
	reinterpret_cast<Bullet*>(temp->GetObjectComponent(bullet))->GetBulletValues().dir = Math::Vec3::unit_y;
	reinterpret_cast<Bullet*>(temp->GetObjectComponent(bullet))->GetBulletValues().speed = 80.0f;
	reinterpret_cast<Bullet*>(temp->GetObjectComponent(bullet))->GetBulletValues().time = 5.0f;
}