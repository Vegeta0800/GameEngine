
#pragma once

//EXTERNAL INCLUDES
#include <list>
//INTERNAL INCLUDES
#include "ra_component.h"
#include "ra_transform.h"

class Gameobject
{
public:
	void Initialize();
	void Update();
	void Cleanup();

	void SetParent(Gameobject* parent);
	void DeleteParent(Gameobject* parent);

	void DeleteChild(Gameobject* child, Gameobject* parent = null);
	void DeleteChildren();

	void MakeRoot();

	void AddChild(Gameobject* child);

	std::list<Gameobject*> GetChildren();
	std::list<Gameobject*> GetAllChildren();
	Gameobject* GetParent();

	bool hasRoot();

private:
	void ListAllChildren(std::list<Gameobject*>& list);


	Transform transform;
	Math::Vec3 oldPosition;

	Gameobject* parent;
	std::list<Gameobject*> children;
	
	std::list<Component*> components;

	bool isRoot = false;
	bool moved = false;
}; 