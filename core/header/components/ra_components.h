
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "math/ra_vector3.h"

//Available components
enum class ComponentType
{
	Player = 0,
	Enemy = 1,
	Bullet = 2,
};

//Values that define the player
struct PlayerValues
{
	float speed;
	float health;
	float bulletSpeed;
	float bulletTimer;
	float bulletTime;
	Math::Vec3 bulletDir;
};

//Values that define the enemy
struct EnemyValues
{
	Math::Vec3 bulletDir;
	float speed;
	float health;
	float bulletSpeed;
	float bulletTimer;
	float bulletTime;
};

//Values that define the bullet
struct BulletValues
{
	Math::Vec3 dir;
	float speed;
	float time;
	bool activated;
};

//Values that define the rigidBody
struct RigidbodyValues
{
	Math::Vec3 gravityDir;
	Math::Vec3 movementDir;

	float mass;
	float airDensity;
	float dragCoefficient;
	float velocity;
	float force;

	bool isEnabled;
	bool isKinematic;
	bool gravityEnabled;
	bool hasCollision;
	bool isColliding;
};
