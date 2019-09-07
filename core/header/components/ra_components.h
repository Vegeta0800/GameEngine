
#pragma once
#include "math/ra_vector3.h"

enum class ComponentType
{
	Movement = 0,
	Shoot = 1,
	Bullet = 2,
	Collision = 3,
	Health = 4,
	GravityShot = 5,
};

struct MovementValues
{
	float stoppingCoefficient;
	float velocity;
	float maxSpeed;
	float speed;
	float rotationSpeed;
	Math::Vec3 direction;
};

struct BulletValues
{
	bool activated;
	float speed;
	float time;
	Math::Vec3 dir;
};

struct ShootingValues
{
	ui64 shotID;
	bool available;
	float timer;
	float cooldown;
	float speed;
	float bulletSpeed;
	float growth;
	float time;
};

struct RigidbodyValues
{
	bool isEnabled;
	bool isKinematic;
	bool gravityEnabled;
	bool hasCollision;
	bool isColliding;

	float mass;
	float airDensity;
	float dragCoefficient;
	float velocity;

	Math::Vec3 gravityDir;
	Math::Vec3 movementDir;
};

struct HealthValues
{
	bool shield;

	float health;
	float damage;
	float resistance;
};


struct RaycastInfo
{
	float distance;
	Math::Vec3 hitPosition;
	Math::Vec3 hitNormal;
};

struct GravityShotValues
{
	float timeInAir;
	float gravityRadius;
	float pullSpeed;
	float cooldown;
	float timeToExplode;

	bool activated;
	float speed;
	float time;
	Math::Vec3 dir;

	Math::Vec3 direction;
	Math::Vec3 gravityDirection;
};
