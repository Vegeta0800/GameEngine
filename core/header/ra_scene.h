
#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include "ra_types.h"
#include "math/ra_vector3.h"

class Gameobject;
class Mesh;
class Material;
class Texture;
class Rigidbody;
class BoxCollider;
class Camera;
class Component;


class Scene
{
public:
	void Initialize(std::string name, Camera* camera = nullptr);
	void Update();
	void Cleanup();

	ui32 GetObjectCount();
	Gameobject* GetSceneRoot();
	Gameobject* GetGameobject(std::string objectName);
	std::vector<Gameobject*> GetAllGameobjects();
	std::vector<std::string> GetObjectPool(std::string poolName);

	std::string GetObjectOfPool(std::string poolName);

	Material* GetMaterial(std::string objectName);
	Mesh* GetMesh(std::string objectName);
	Camera* GetCamera();
	Rigidbody* GetRigidBody(std::string objectName);
	BoxCollider* GetBoxCollider(std::string objectName);

	Component* GetObjectComponent(std::string objectName);

	void AddObject(std::string name, std::string modelPath, bool active = true, std::vector<std::string> texturePaths = std::vector<std::string>(), std::string parentName = "");
	void AddObjectPool(std::string objectPoolName, ui32 poolSize, std::string name, std::string modelPath, bool active = true, std::vector<std::string> texturePaths = std::vector<std::string>(), std::string parentName = "");
	void AddComponent(Component* component, std::string name);
	void AddComponentToObjectPool(std::string name, Component* component);

	void SetActive(std::string objectName, bool active);

	bool isProjectionIntersecting(Math::Vec3 aCorners[], Math::Vec3 bCorners[], Math::Vec3 axis);
	Math::Vec3 GetAxis(Math::Vec3 point1, Math::Vec3 point2);
private:
	void UpdateCollisions();
	bool CheckCollision(Gameobject* gb, Gameobject* gbB);

	ui32 objectCount;

	std::string sceneName;

	Camera* mainCamera;

	std::vector<std::string> objects;
	std::vector<Gameobject*> gameObjectVector;
	std::vector<Component*> components;

	std::unordered_map<std::string, Component*> componentMap;
	std::unordered_map<std::string, Gameobject*> gameObjects;
	std::unordered_map<std::string, std::vector<std::string>> objectPooledObjects;
	std::unordered_map<std::string, Mesh*> meshes;
	std::unordered_map<std::string, Material*> materials;
	std::unordered_map<std::string, Rigidbody*> rigidBodies;
	std::unordered_map<std::string, BoxCollider*> boxCollider;
};