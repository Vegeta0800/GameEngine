#pragma once

//EXTERNAL INCLUDES
#include <list>
//INTERNAL INCLUDES
#include "math/ra_mat4x4.h"
#include "ra_transform.h"
#include "components/ra_components.h"

class Component;

class Gameobject
{
public:
	Gameobject();
	void Initialize(Gameobject* parent = nullptr, std::string name = "");
	void Update();
	void Cleanup();
	
	void SetParent(Gameobject* parent);
	void SetMesh(const char* meshName);
	void SetName(std::string name = "");
	void DeleteParent();

	void DeleteChild(Gameobject* child, Gameobject* parent = nullptr);
	void DeleteChildren();

	void MakeRoot();

	void AddChild(Gameobject* child);
	void AddComponent(Component* component);

	void SetHitObject(Gameobject* gb);

	std::list<Gameobject*> GetChildren();
	std::list<Gameobject*> GetAllChildren();
	Gameobject* GetParent();
	Gameobject* GetHitObject();
	Transform& GetTransform();
	Math::Mat4x4 GetModelMatrix();

	ui32& GetModelID();

	std::string GetName();

	bool hasRoot();
	bool isMoved();

	bool& hasCollision();
	bool& isTrigger();
	bool& isColliding();
	bool& GetIsRenderable();
	bool& GetIsActive();
	bool& GetIsInstanced();
	bool& GetIsInFrustum();
	bool& GetBufferCreated();

	bool operator==(Gameobject* other);

private:
	Gameobject* hitObject = nullptr;

	void ListAllChildren(std::list<Gameobject*>& list);

	std::string name;
	
	Transform transform;
	
	Math::Vec3 oldPosition;
	Math::Vec3 oldEulerRotation;

	Math::Mat4x4 modelMatrix;

	Gameobject* parent;

	ui32 modelID;

	std::list<Gameobject*> children;
	std::list<Component*> components;

	bool isRoot = false;
	bool moved = false;
	bool renderable = true;
	bool bufferCreated = false;
	bool active = true;
	bool instanced = false;
	bool inFrustum = true;

	bool collision = true;
	bool colliding = false;
	bool trigger = false;
}; 