
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "components/player/ra_player.h"
#include "components/player/ra_bullet.h"
#include "components/ra_component.h"
#include "physic/ra_rigidbody.h"
#include "input/ra_inputhandler.h"
#include "ra_application.h"
#include "ra_scenemanager.h"
#include "ra_scene.h"
#include "ra_material.h"
#include "ra_transform.h"

//Default constructor
Player::Player()
{

}


//Initialize player with standard values
void Player::Initialize(Rigidbody* rigidBody)
{
	//Set Player type
	Component::Initialize("Player", ComponentType::Player);
	this->rigidBody = rigidBody;
	this->player.speed = 30.0f;
	this->player.health = 100.0f;
}
//Update players movement and shooting behavior
void Player::Update()
{
	Component::Update();

	//Move player
	Move();

	//If Space is released
	if (Input::GetInstancePtr()->GetKeyUp(KeyCode::Space))
	{
		//Shoot and set shoot in my data to true.
		Shoot();
		Input::GetInstancePtr()->GetMyData().shoot = true;
	}
	else
	{
		//Set shoot in my data to false.
		Input::GetInstancePtr()->GetMyData().shoot = false;
	}
}
//Cleanup player
void Player::Cleanup()
{
	this->rigidBody = nullptr;
	Component::Cleanup();
}


//Get reference to players values
PlayerValues& Player::GetPlayerValues(void)
{
	return this->player;
}


//Players move behavior
void Player::Move()
{
	//If Key A is hold down
	if (Input::GetInstancePtr()->GetKey(KeyCode::A))
	{
		//Move left and set left in my data to true.
		this->rigidBody->AddForce(Math::Vec3::neg_unit_x, this->player.speed);
		Input::GetInstancePtr()->GetMyData().left = true;
	}
	//Else if Key D is hold down
	else if (Input::GetInstancePtr()->GetKey(KeyCode::D))
	{
		//Move right and set right in my data to true.
		this->rigidBody->AddForce(Math::Vec3::unit_x, this->player.speed);
		Input::GetInstancePtr()->GetMyData().right = true;
	}
	else
	{
		//set right in my data to false.
		Input::GetInstancePtr()->GetMyData().right = false;
		//set left in my data to false.
		Input::GetInstancePtr()->GetMyData().left = false;
	}
}
//Players shooting behavior
void Player::Shoot()
{
	//Get bullet name from bullets pool in current scene
	Scene* temp = SceneManager::GetInstancePtr()->GetActiveScene();
	std::string bullet = temp->GetObjectOfPool("bullets");
	
	//Set it active
	temp->SetActive(bullet, true);
	
	//Set transform values
	temp->GetRigidBody(bullet)->GetTransform()->position = this->rigidBody->GetTransform()->position;
	temp->GetRigidBody(bullet)->GetTransform()->scaling.x = 0.5f;
	temp->GetRigidBody(bullet)->GetTransform()->scaling.z = 0.5f;
	temp->GetRigidBody(bullet)->GetTransform()->scaling.y = 0.3f;
	
	//Color blue
	temp->GetMaterial(bullet)->fragColor = fColorRGBA{ 0.117f, 0.474f, 0.862f, 1.0f };
	
	//Set bullet values.
	Bullet* tempBullet = reinterpret_cast<Bullet*>(temp->GetObjectComponent(bullet, ComponentType::Bullet));
	tempBullet->SetParent(this->rigidBody);
	tempBullet->GetBulletValues().activated = true;
	tempBullet->GetBulletValues().dir = Math::Vec3::unit_y;
	tempBullet->GetBulletValues().speed = 80.0f;
	tempBullet->GetBulletValues().time = 5.0f;
}