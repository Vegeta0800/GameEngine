
#pragma once
//EXTERNAL INCLUDES
#include <string>
//INTERNAL INCLUDES
#include "components/ra_component.h"

class Rigidbody;

//Class Movement that is derived from class Component.
class Player : public Component
{
	//Declare the public functions that are overwritten.
public:
	Player();

	void Initialize(Rigidbody* rigidBody);
	virtual void Update(void) override;
	virtual void Cleanup(void) override;

	PlayerValues& GetPlayerValues(void);

private:
	void Move();
	void Shoot();

	PlayerValues player;
	Rigidbody* rigidBody;
};
