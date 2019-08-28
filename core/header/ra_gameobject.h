
#pragma once

//EXTERNAL INCLUDES
#include <list>
//INTERNAL INCLUDES
#include "math/ra_mat4x4.h"
#include "ra_mesh.h"
#include "ra_component.h"
#include "ra_transform.h"
#include "ra_material.h"
#include "ra_boxcollider.h"

class Texture;

class Gameobject
{
public:
	Gameobject();
	void Initialize(Gameobject* parent = nullptr, std::string name = "", const char* meshName = "", const std::vector<const char*>& textureNames = std::vector<const char*>(), bool render = false, bool act = true, bool instanced = false);
	void Initialize(Gameobject* copyGb, std::string name = "", bool render = false);
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

	std::list<Gameobject*> GetChildren();
	std::list<Gameobject*> GetAllChildren();
	Gameobject* GetParent();
	Transform& GetTransform();
	Mesh& GetMesh();
	std::vector<Texture*> GetTextures();
	BoxCollider* GetBoxCollider();
	Math::Mat4x4 GetModelMatrix();
	Material& GetMaterial();

	ui32& GetModelID();

	std::string GetName();
	const char* GetMeshName();
	const char* GetTextureName();
	const char* GetNormalMapName();
	const char* GetEmissionMapName();
	const char* GetRoughnessMapName();
	const char* GetAmbientMapName();

	bool hasRoot();
	bool isMoved();

	bool& GetIsRenderable();
	bool& GetIsActive();
	bool& GetIsInstanced();
	bool& GetIsInFrustum();
	bool& GetBufferCreated();

	bool operator==(Gameobject* other);

private:
	void ListAllChildren(std::list<Gameobject*>& list);

	std::string name;
	const char* meshName;
	const char* textureName;
	const char* normalMapName;
	const char* emissionMapName;
	const char* roughnessMapName;
	const char* ambientMapName;

	Transform transform;
	
	Math::Vec3 oldPosition;
	Math::Vec3 oldEulerRotation;

	Math::Mat4x4 modelMatrix;

	Gameobject* parent;

	Material material;

	Mesh mesh;
	std::vector<Texture*> textures;

	BoxCollider* collider;

	ui32 modelID;

	std::list<Gameobject*> children;
	std::list<Component*> components;

	bool isRoot = false;
	bool moved = false;
	bool renderable = false;
	bool bufferCreated = false;
	bool active = false;
	bool instanced = false;
	bool inFrustum = true;
}; 