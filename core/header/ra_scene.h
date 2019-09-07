
#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include "ra_types.h"

class Gameobject;
class Mesh;
class Material;
class Texture;
class Rigidbody;
class BoxCollider;


class Scene
{
public:
	void Initialize(std::string name);
	void Update();
	void Cleanup();

	ui32 GetObjectCount();
	Gameobject* GetSceneRoot();
	Gameobject* GetGameobject(std::string objectName);
	Material* GetMaterial(std::string objectName);
	Mesh* GetMesh(std::string objectName);
	BoxCollider* GetBoxCollider(std::string objectName);

	void AddObject(std::string name, std::string modelPath, std::vector<std::string> texturePaths, std::string parentName = "");
private:
	ui32 objectCount;

	std::string sceneName;

	std::vector<std::string> objects;

	std::unordered_map<std::string, Gameobject*> gameObjects;
	std::unordered_map<std::string, Mesh*> meshes;
	std::unordered_map<std::string, Material*> materials;
	std::unordered_map<std::string, Rigidbody*> rigidBodies;
	std::unordered_map<std::string, BoxCollider*> boxCollider;
};