
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "components/player/ra_bullet.h"
#include "components/ra_component.h"
#include "components/player/ra_player.h"
#include "components/enemy/ra_enemy.h"
#include "physic/ra_rigidbody.h"
#include "ra_application.h"
#include "ra_scene.h"
#include "ra_scenemanager.h"

//On normal creation
Bullet::Bullet()
{
}
//On creation with a reference bullet.
Bullet::Bullet(Bullet* comp)
{
	this->bullet = comp->bullet;
}


//Initialize bullet with its rigidBody
void Bullet::Initialize(Rigidbody* rigidBody)
{
	Component::Initialize("Bullet", ComponentType::Bullet);
	this->rigidBody = rigidBody;
}
//Update each frame
void Bullet::Update(void)
{
	//Execute components update
	Component::Update();

	//If the bullet is active
	if (this->bullet.activated)
	{
		//And still allowed to fly
		if (this->bullet.time > 0.0f)
		{
			//Reduce time to fly
			this->bullet.time -= 1.0f * Application::GetInstancePtr()->GetDeltaTime();

			//If the bullet is colliding
			if (this->rigidBody->GetHitObject() != nullptr)
			{
				//If its with its parent, continue flying
				if (this->rigidBody->GetHitObject() == this->parentRigidBody)
				{
					this->rigidBody->AddForce(this->bullet.dir, this->bullet.speed);
				}
				//If not
				else
				{
					//If collided with the enemy
					if (this->rigidBody->GetHitObject()->GetName() == "Enemy")
					{
						//Get health as a reference from the hit objects enemy script
						float& health = reinterpret_cast<Enemy*>(SceneManager::GetInstancePtr()->GetActiveScene()
							->GetObjectComponent(this->rigidBody->GetHitObject()->GetName(), ComponentType::Enemy)
							)->GetEnemyValues().health;

						//Reduce that health by a factor //TODO
						health -= 25.0f;

						//If health drops under 0
						if (health <= 0.0f)
						{
							//Deactivate the enemy
							SceneManager::GetInstancePtr()->GetActiveScene()
								->SetActive(this->rigidBody->GetHitObject()->GetName(), false);

							//End game and send result
							reinterpret_cast<Enemy*>(SceneManager::GetInstancePtr()->GetActiveScene()
								->GetObjectComponent(this->rigidBody->GetHitObject()->GetName(), ComponentType::Enemy)
								)->Death();
						}

						//Set the bullets hitObject to nullptr
						this->rigidBody->SetHitObject(nullptr);

						//Deactivate the bullet
						SceneManager::GetInstancePtr()->GetActiveScene()->SetActive(this->rigidBody->GetName(), false);
						return; // Done for good.
					}
					//If collided with the player
					else if (this->rigidBody->GetHitObject()->GetName() == "Player")
					{
						//Get health as a reference from the hit objects player script
						float& health = reinterpret_cast<Player*>(SceneManager::GetInstancePtr()->GetActiveScene()
							->GetObjectComponent(this->rigidBody->GetHitObject()->GetName(), ComponentType::Player)
							)->GetPlayerValues().health;

						//Reduce that health by a factor //TODO
						health -= 25.0f;

						//If health drops under 0
						if (health <= 0.0f)
						{
							//Deactivate the player
							SceneManager::GetInstancePtr()->GetActiveScene()
								->SetActive(this->rigidBody->GetHitObject()->GetName(), false);

							//End game
							reinterpret_cast<Player*>(SceneManager::GetInstancePtr()->GetActiveScene()
								->GetObjectComponent(this->rigidBody->GetHitObject()->GetName(), ComponentType::Player)
								)->Death();
						}

						//Set the bullets hitObject to nullptr
						this->rigidBody->SetHitObject(nullptr);

						//Deactivate the bullet
						SceneManager::GetInstancePtr()->GetActiveScene()->SetActive(this->rigidBody->GetName(), false);
						return; //Done for good
					}
				}
			}
			//If not colliding
			else
			{
				//Go on and fly
				this->rigidBody->AddForce(this->bullet.dir, this->bullet.speed);
			}
		}
		//If time has run out for the bullet
		else
		{
			//Deactivate the bullet and its script
			SceneManager::GetInstancePtr()->GetActiveScene()->SetActive(this->rigidBody->GetName(), false);
			this->bullet.activated = false;
		}
	}
}
//Cleanup the bullet
void Bullet::Cleanup(void)
{
	this->parentRigidBody = nullptr;
	this->rigidBody = nullptr;

	Component::Cleanup();
}


//Set the bullets parent
void Bullet::SetParent(Rigidbody* parent)
{
	this->parentRigidBody = parent;
}


//Get the bullets values
BulletValues& Bullet::GetBulletValues(void)
{
	return this->bullet;
}