
#pragma once

//EXTERNAL INCLUDES
#include <list>
//INTERNAL INCLUDES
#include "math/ra_mat4x4.h"
#include "ra_component.h"
#include "ra_transform.h"
#include "ra_mesh.h"

class Gameobject
{
public:
	Gameobject();
	void Initialize();
	void Update();
	void Cleanup();

	void SetParent(Gameobject* parent);
	void SetName(const char* name = "");
	void DeleteParent();

	void DeleteChild(Gameobject* child, Gameobject* parent = nullptr);
	void DeleteChildren();

	void MakeRoot();

	void AddChild(Gameobject* child);
	void AddComponent(Component* component);

	std::list<Gameobject*> GetChildren();
	std::list<Gameobject*> GetAllChildren();
	bool operator==(Gameobject* other);
	Gameobject* GetParent();
	Transform& GetTransform();
	Math::Mat4x4 GetModelMatrix();

	bool hasRoot();
	bool isMoved();

	Mesh* GetMesh();

private:
	void ListAllChildren(std::list<Gameobject*>& list);

	const char* name;

	Transform transform;
	Math::Vec3 oldPosition;
	Math::Vec3 oldEulerRotation;

	Math::Mat4x4 modelMatrix;

	Gameobject* parent;
	Mesh* mesh;

	std::list<Gameobject*> children;
	
	std::list<Component*> components;

	bool isRoot = false;
	bool moved = false;
}; 