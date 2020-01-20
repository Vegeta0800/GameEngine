
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "components/ra_component.h"

class Rigidbody;

//Class Movement that is derived from class Component.
class Player : public Component
{
	//Declare the public functions that are overwritten.
public:
	//Default constructor
	Player();


	//Initialize player with standard values
	void Initialize(Rigidbody* rigidBody);
	//Update players movement and shooting behavior
	virtual void Update(void) override;
	//Cleanup player
	virtual void Cleanup(void) override;

	//If player is killed end game
	void Death();


	//Get reference to players values
	PlayerValues& GetPlayerValues(void);

private:
	//Players move behavior
	void Move();
	//Players shooting behavior
	void Shoot();

	PlayerValues player;
	Rigidbody* rigidBody;
};
