
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "components/ra_component.h"

class Rigidbody;

//Class Bullet that is derived from class Component.
class Bullet : public Component
{
	//Declare the public functions that are overwritten.
public:
	//On normal creation
	Bullet();
	//On creation with a reference bullet.
	Bullet(Bullet* comp);

	//Initialize bullet with its rigidBody
	void Initialize(Rigidbody* rigidBody);
	//Update each frame
	virtual void Update(void) override;
	//Cleanup the bullet
	virtual void Cleanup(void) override;


	//Set the bullets parent
	void SetParent(Rigidbody* parent);


	//Get the bullets values
	BulletValues& GetBulletValues(void);
private:
	BulletValues bullet;
	Rigidbody* parentRigidBody;
	Rigidbody* rigidBody;
};