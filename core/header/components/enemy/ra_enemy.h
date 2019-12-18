
#pragma once
//EXTERNAL INCLUDES
#include <string>
//INTERNAL INCLUDES
#include "components/ra_component.h"

class Rigidbody;

//Class Movement that is derived from class Component.
class Enemy : public Component
{
	//Declare the public functions that are overwritten.
public:
	Enemy();

	void Initialize(Rigidbody* rigidBody);
	virtual void Update(void) override;
	virtual void Cleanup(void) override;


private:
	void Move();
	void Shoot();

	EnemyValues enemy;
	Rigidbody* rigidBody;
};