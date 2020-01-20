
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "components/ra_component.h"

class Rigidbody;

//Class Enemy that is derived from class Component.
class Enemy : public Component
{
	//Declare the public functions that are overwritten.
public:
	//Default constructor
	Enemy();


	//Initialize player with standard values
	void Initialize(Rigidbody* rigidBody);
	//Update players movement and shooting behavior
	virtual void Update(void) override;
	//Cleanup enemy
	virtual void Cleanup(void) override;

	//If enemy is killed send result to server and end game
	void Death();

	//Get Enemy values
	EnemyValues& GetEnemyValues();

private:
	//Move left or right when client gets opponents data to do so from the network
	void Move();
	//Shoot function, gets bullet, shoots bullet
	void Shoot();

	EnemyValues enemy;
	Rigidbody* rigidBody;
};