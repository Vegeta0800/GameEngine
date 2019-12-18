#pragma once
//EXTERNAL INCLUDES
#include <string>
//INTERNAL INCLUDES
#include "components/ra_component.h"

class Rigidbody;

//Class Movement that is derived from class Component.
class Bullet : public Component
{
	//Declare the public functions that are overwritten.
public:
	Bullet();
	Bullet(Bullet* comp);

	void Initialize(Rigidbody* rigidBody);

	void SetParent(Rigidbody* rigidBody);

	virtual void Update(void) override;
	virtual void Cleanup(void) override;

	BulletValues& GetBulletValues(void);
private:
	BulletValues bullet;
	Rigidbody* rigidBody;
	Rigidbody* parentRigidBody;
};