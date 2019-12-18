#include "components/ra_component.h"
#include "components/player/ra_bullet.h"
#include "physic/ra_rigidbody.h"
#include "ra_application.h"
#include "ra_scene.h"
#include "ra_scenemanager.h"

Bullet::Bullet()
{
}

Bullet::Bullet(Bullet* comp)
{
	this->bullet = comp->bullet;
}

void Bullet::Initialize(Rigidbody* rigidBody)
{
	Component::Initialize("Bullet", ComponentType::Bullet);
	this->rigidBody = rigidBody;
}

void Bullet::SetParent(Rigidbody* rigidBody)
{
	this->parentRigidBody = rigidBody;
}

void Bullet::Update(void)
{
	Component::Update();

	if (this->bullet.activated)
	{
		if (this->bullet.time > 0.0f)
		{
			this->bullet.time -= 1.0f * Application::GetInstancePtr()->GetDeltaTime();

			if (this->rigidBody->GetHitObject() != nullptr)
			{
				if (this->rigidBody->GetHitObject() == this->parentRigidBody)
				{
					this->rigidBody->AddForce(this->bullet.dir, this->bullet.speed);
				}
			}
			else
				this->rigidBody->AddForce(this->bullet.dir, this->bullet.speed);
		}
		else
		{
			SceneManager::GetInstancePtr()->GetActiveScene()->SetActive(this->rigidBody->GetName(), false);
			this->bullet.activated = false;
		}
	}
}

void Bullet::Cleanup(void)
{
	Component::Cleanup();
}

BulletValues& Bullet::GetBulletValues(void)
{
	return this->bullet;
}
