
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "components/enemy/ra_enemy.h"
#include "components/ra_component.h"
#include "components/player/ra_bullet.h"
#include "input/ra_inputhandler.h"
#include "physic/ra_rigidbody.h"
#include "ra_application.h"
#include "ra_scenemanager.h"
#include "ra_scene.h"
#include "ra_material.h"
#include "ra_transform.h"

//Default constructor
Enemy::Enemy()
{

}


//Initialize player with standard values
void Enemy::Initialize(Rigidbody* rigidBody)
{
	Component::Initialize("Enemy", ComponentType::Enemy);
	this->rigidBody = rigidBody;
	this->enemy.speed = 30.0f;
	this->enemy.health = 100.0f;
}
//Update players movement and shooting behavior
void Enemy::Update()
{
	Component::Update();

	Move();

	//TODO
	if (Input::GetInstancePtr()->GetOpponentData().shoot)
	{
		Input::GetInstancePtr()->GetOpponentData().shoot = false;
		Shoot();
	}
}
//Cleanup enemy
void Enemy::Cleanup()
{
	this->rigidBody = nullptr;
	Component::Cleanup();
}

//If enemy is killed send result to server and end game
void Enemy::Death()
{
	Application::GetInstancePtr()->GetWinnerState() = true;
	Application::GetInstancePtr()->GetRunState() = false;
}


//Get Enemy values
EnemyValues& Enemy::GetEnemyValues(void)
{
	return this->enemy;
}


//Move left or right when client gets opponents data to do so from the network
void Enemy::Move()
{
	if (Input::GetInstancePtr()->GetOpponentData().left)
		this->rigidBody->AddForce(Math::Vec3::unit_x, this->enemy.speed);
	if (Input::GetInstancePtr()->GetOpponentData().right)
		this->rigidBody->AddForce(Math::Vec3::neg_unit_x, this->enemy.speed);
}
//Shoot function, gets bullet, shoots bullet
void Enemy::Shoot()
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
	temp->GetRigidBody(bullet)->GetTransform()->eulerRotation.z = 180.0f;

	//Color red
	temp->GetMaterial(bullet)->fragColor = fColorRGBA{ 0.99f, 0.22f, 0.04f, 1.0f };
	
	//Set bullet values.
	Bullet* tempBullet = reinterpret_cast<Bullet*>(temp->GetObjectComponent(bullet, ComponentType::Bullet));
	tempBullet->SetParent(this->rigidBody);
	tempBullet->GetBulletValues().activated = true;
	tempBullet->GetBulletValues().dir = Math::Vec3::neg_unit_y;
	tempBullet->GetBulletValues().speed = 80.0f;
	tempBullet->GetBulletValues().time = 5.0f;

	temp = nullptr;
	tempBullet = nullptr;
}