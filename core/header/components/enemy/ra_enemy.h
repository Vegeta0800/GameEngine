
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
	Enemy();

	void Initialize(Rigidbody* rigidBody);
	virtual void Update(void) override;
	virtual void Cleanup(void) override;

	EnemyValues& GetEnemyValues();
private:
	void Move();
	void Shoot();

	EnemyValues enemy;
	Rigidbody* rigidBody;
};