#include "components/ra_component.h"
#include "components/player/ra_player.h"
#include "input/ra_inputhandler.h"
#include "physic/ra_rigidbody.h"

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
	}

	if (Input::GetInstancePtr()->GetKey(KeyCode::D))
	{
		this->rigidBody->AddForce(Math::Vec3::unit_x, this->player.speed);
	}
}

void Player::Rotate(void)
{
}
