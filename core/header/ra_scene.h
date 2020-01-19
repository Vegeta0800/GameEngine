
#pragma once
//EXTERNAL INCLUDES
#include <unordered_map>
#include <string>
#include <vector>
//INTERNAL INCLUDES
#include "ra_types.h"
#include "math/ra_vector3.h"
#include "components/ra_components.h"

//Forward declarations
class Gameobject;
class Mesh;
class Material;
class Texture;
class Rigidbody;
class Camera;
class Component;

//Scene class that holds all objects and components
class Scene
{
public:
	//Initialize scene, camera and setup root node
	void Initialize(std::string name, Camera* camera = nullptr);
	//Update scene graph, collisions, camera, components and rigidbodies
	void Update();
	//Update collisions on all active gameobjects
	void UpdateCollisions();
	//Cleanup Scenegraph, components, rigidbodies, camera, meshes
	void Cleanup();


	//Add a object to a scene
	void AddObject(std::string name, std::string modelPath, bool active = true, std::vector<std::string> texturePaths = std::vector<std::string>(), std::string parentName = "");
	//Add object pool
	void AddObjectPool(std::string objectPoolName, ui32 poolSize, std::string name, std::string modelPath, bool active = true, std::vector<std::string> texturePaths = std::vector<std::string>(), std::string parentName = "");
	//Add component to object
	void AddComponent(Component* component, std::string name);
	//Add component to all objects in an object pool
	void AddComponentToObjectPool(std::string name, Component* component);


	//Set object to active
	void SetActive(std::string objectName, bool active);


	//Get all gameobjects in scene
	std::vector<Gameobject*> GetAllGameobjects();
	//Get all components of object
	std::vector<Component*> GetObjectComponents(std::string objectName);
	//Get a object pool
	std::vector<std::string> GetObjectPool(std::string poolName);
	
	//Get gameobject pointer to the scene root
	Gameobject* GetSceneRoot();
	//Get specific game object
	Gameobject* GetGameobject(std::string objectName);
	//Get specific rigidbody
	Rigidbody* GetRigidBody(std::string objectName);
	//Get specific component of an object
	Component* GetObjectComponent(std::string objectName, ComponentType type);
	//Get material of object
	Material* GetMaterial(std::string objectName);
	//Get mesh of object
	Mesh* GetMesh(std::string objectName);
	//Get scenes camera
	Camera* GetCamera();
	
	//Get a first object in an objectPool
	std::string GetObjectOfPool(std::string poolName);
	//Get Object count
	ui32 GetObjectCount();

private:

	//Check collision between two gameobjects
	bool CheckCollision(Gameobject* gb, Gameobject* gbB);

	std::vector<Gameobject*> gameObjectVector;
	std::vector<Component*> components;
	std::vector<std::string> objects;

	std::unordered_map<std::string, std::vector<std::string>> objectPooledObjects;
	std::unordered_map<std::string, Gameobject*> gameObjects;
	std::unordered_map<std::string, std::vector<Component*>> componentMap;
	std::unordered_map<std::string, Rigidbody*> rigidBodies;
	std::unordered_map<std::string, Material*> materials;
	std::unordered_map<std::string, Mesh*> meshes;

	std::string sceneName;

	float radiusReduction = 3.0f;

	Camera* mainCamera;
	ui32 objectCount;
};