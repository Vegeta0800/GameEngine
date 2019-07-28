
#pragma once

//EXTERNAL INCLUDES
#include <list>
//INTERNAL INCLUDES
#include "math/ra_mat4x4.h"
#include "ra_mesh.h"
#include "ra_component.h"
#include "ra_transform.h"
#include "ra_material.h"

class Texture;

class Gameobject
{
public:
	Gameobject();
	void Initialize(Gameobject* parent = nullptr, const char* name = "", const char* meshName = "", const char* textureName = "", bool render = false, bool act = true);
	void Initialize(Gameobject* copyGb, const char* name = "", bool copyAll = false);
	void Update();
	void Cleanup();
	
	void SetParent(Gameobject* parent);
	void SetMesh(const char* meshName);
	void SetName(const char* name = "");
	void DeleteParent();

	void DeleteChild(Gameobject* child, Gameobject* parent = nullptr);
	void DeleteChildren();

	void MakeRoot();

	void AddChild(Gameobject* child);
	void AddComponent(Component* component);

	std::list<Gameobject*> GetChildren();
	std::list<Gameobject*> GetAllChildren();
	Gameobject* GetParent();
	Transform& GetTransform();
	Mesh& GetMesh();
	Texture* GetTexture();
	Math::Mat4x4 GetModelMatrix();
	Material& GetMaterial();

	const char* GetName();
	const char* GetMeshName();
	const char* GetTextureName();

	bool hasRoot();
	bool isMoved();

	bool& GetIsRenderable();
	bool& GetIsActive();

	bool operator==(Gameobject* other);

private:
	void ListAllChildren(std::list<Gameobject*>& list);

	const char* name;
	const char* meshName;
	const char* textureName;

	Transform transform;
	
	Math::Vec3 oldPosition;
	Math::Vec3 oldEulerRotation;

	Math::Mat4x4 modelMatrix;

	Gameobject* parent;

	Material material;

	Mesh mesh;
	Texture* texture;

	std::list<Gameobject*> children;
	std::list<Component*> components;

	bool isRoot = false;
	bool moved = false;
	bool renderable = false;
	bool active = false;
}; 