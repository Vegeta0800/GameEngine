
//EXTERNAL INCLUDES
#include <unordered_set>
#include <thread>
//INTERNAL INCLUDES
#include "ra_scene.h"
#include "math/ra_mathfunctions.h"
#include "input/ra_inputhandler.h"
#include "filesystem/ra_filesystem.h"
#include "components/player/ra_player.h"
#include "components/player/ra_bullet.h"
#include "components/enemy/ra_enemy.h"
#include "physic/ra_rigidbody.h"
#include "ra_application.h"
#include "ra_camera.h"
#include "ra_gameobject.h"
#include "ra_mesh.h"
#include "ra_texture.h"
#include "ra_material.h"

//Initialize scene, camera and setup root node
void Scene::Initialize(std::string name, Camera* camera)
{
	//Set scene name and create root node
	this->sceneName = name;
	Gameobject* scene = new Gameobject;
	scene->SetName(name);
	scene->hasRoot() = true;

	this->gameObjects[name] = scene;

	//Create camera or set camera if possible
	if (camera == nullptr)
	{
		this->mainCamera = new Camera;
		this->mainCamera->Initialize();
	}
	else
		this->mainCamera = camera;
}
//Update scene graph, collisions, camera, components and rigidbodies
void Scene::Update()
{
	//Update collision
	UpdateCollisions();

	//Loop through components cast to type and update it
	for (Component* comp : this->components)
	{
		switch (comp->GetType())
		{
		case ComponentType::Player:
			reinterpret_cast<Player*>(comp)->Update();
			break;
		case ComponentType::Bullet:
			reinterpret_cast<Bullet*>(comp)->Update();
			break;
		case ComponentType::Enemy:
			reinterpret_cast<Enemy*>(comp)->Update();
			break;
		}
	}

	//Loop through all rigidbodies and update them if active
	for (std::string object : this->objects)
	{
		if(this->gameObjects[object]->GetIsActive())
			this->rigidBodies[object]->Update();
	}

	//Update scene root (in turn updates whole scene graph)
	this->gameObjects[this->sceneName]->Update();

	//Update camera
	this->mainCamera->Update();
}
//Update collisions on all active gameobjects
void Scene::UpdateCollisions()
{
	//Setup checked gameobjects hash set
	std::unordered_set<Gameobject*> checkedGameobjects;
	//Setup already colliding gameobjects in this iteration
	std::unordered_map<Gameobject*, bool> collided;
	
	//For all gameobjects
	for (int i = 0; i < this->gameObjectVector.size(); i++)
	{
		//Check if it has collision and it is active
		if (this->gameObjectVector[i]->hasCollision() && this->gameObjectVector[i]->GetIsActive())
		{
			//Then loop through all gameobjects that are:
			for (int j = 0; j < this->gameObjectVector.size(); j++)
			{
				//Not current gameobject, active, collidable, without the same tag and not collided yet in this iteration
				if (j != i && this->gameObjectVector[j]->GetIsActive()
					&& this->gameObjectVector[i]->hasCollision()
					&& this->gameObjectVector[i]->GetTag() != this->gameObjectVector[j]->GetTag()
					&& !collided[this->gameObjectVector[i]] && !collided[this->gameObjectVector[j]])
				{
					//Check if that gameobject already is not in the checked gameobjects hashset
					if (checkedGameobjects.find(this->gameObjectVector[j]) == checkedGameobjects.end())
					{
						//If it is not check collision between current and other gameobject
						if (CheckCollision(this->gameObjectVector[i], this->gameObjectVector[j]))
						{
							//If they collide, set their rigidbody colliding bools to true
							this->rigidBodies[this->gameObjectVector[i]->GetName()]->GetRigidbodyValues().isColliding = true;
							this->rigidBodies[this->gameObjectVector[j]->GetName()]->GetRigidbodyValues().isColliding = true;

							//Set their rigidbodies hitObjects to each other.
							this->rigidBodies[this->gameObjectVector[i]->GetName()]
								->SetHitObject(this->rigidBodies[this->gameObjectVector[j]->GetName()]);
							this->rigidBodies[this->gameObjectVector[j]->GetName()]
								->SetHitObject(this->rigidBodies[this->gameObjectVector[i]->GetName()]);
							
							//Prevent from being checked again this iteration
							collided[this->gameObjectVector[j]] = true;
							collided[this->gameObjectVector[i]] = true;
						}
						//If they are not colliding
						else
						{
							//Set their rigidbody colliding bools to false
							this->rigidBodies[this->gameObjectVector[i]->GetName()]->GetRigidbodyValues().isColliding = false;
							this->rigidBodies[this->gameObjectVector[j]->GetName()]->GetRigidbodyValues().isColliding = false;

							//Set their rigidbodies hitObjects to nullptr.
							this->rigidBodies[this->gameObjectVector[i]->GetName()]->SetHitObject(nullptr);
							this->rigidBodies[this->gameObjectVector[j]->GetName()]->SetHitObject(nullptr);
						}
					}
				}
			}
			
			//Prevent current gameobject from being checked again.
			checkedGameobjects.insert(this->gameObjectVector[i]);
		}
	}
}
//Cleanup Scenegraph, components, rigidbodies, camera, meshes
void Scene::Cleanup()
{
	//For all components cast to its type, execute cleanup and delete it.
	for (Component* comp : this->components)
	{
		switch (comp->GetType())
		{
		case ComponentType::Player:
		{
			reinterpret_cast<Player*>(comp)->Cleanup();
			break;
		}
		case ComponentType::Bullet:
		{
			reinterpret_cast<Bullet*>(comp)->Cleanup();
			break;
		}
		case ComponentType::Enemy:
		{
			reinterpret_cast<Enemy*>(comp)->Cleanup();
			break;
		}
		}
	}

	//Cleanup all rigidbodies and meshes.
	for (std::string object : this->objects)
	{
		this->rigidBodies[object]->Cleanup();
		this->meshes[object]->Cleanup();
	}

	//Delete camera
	delete this->mainCamera->GetTarget();
	delete this->mainCamera;

	//Cleanup scenegraph
	this->gameObjects[this->sceneName]->Cleanup();

	//Clear maps and vectors
	this->gameObjectVector.clear();
	this->objectPooledObjects.clear();
	this->gameObjects.clear();
	this->componentMap.clear();
	this->meshes.clear();
	this->rigidBodies.clear();
	this->materials.clear();
	this->components.clear();
	this->objects.clear();
}


//Add a object to a scene
void Scene::AddObject(std::string name, std::string modelPath, bool active, std::vector<std::string> texturePaths, std::string parentName)
{
	//If active then increment the object count
	if (active)
		this->objectCount++;


	//Create new gameobject and initialize it either with a parent or without a parent
	this->gameObjects[name] = new Gameobject();
	if (parentName != "")
		this->gameObjects[name]->Initialize(this->gameObjects[parentName], name, active);
	else
		this->gameObjects[name]->Initialize(this->gameObjects[this->sceneName], name, active);


	//Create objects textures
	std::vector<Texture*> objectTextures(5);
	std::string path = Application::GetInstancePtr()->GetFilesystem()->DirectoryPath("textures"); //Get texture file path
	path.pop_back(); //Remove last /

	//If there are no specific texture paths given to the function
	if (texturePaths.size() == 0) 
	{
		//Initialize everything with a white image
		for (int i = 0; i < objectTextures.size(); i++)
		{
			//Create and set new texture.
			Texture* tex = new Texture();
			tex->GetPath() = path + "empty.png";
			objectTextures[i] = tex;
		}
	}
	//If there are specific texture paths
	else
	{
		//Set those
		for (int i = 0; i < texturePaths.size(); i++)
		{
			Texture* tex = new Texture();
			texturePaths[i] = path + texturePaths[i] + ".png";
			tex->GetPath() = texturePaths[i];
			objectTextures[i] = tex;
		}
	}


	//Create new mesh with the input model path.
	modelPath = modelPath + ".obj";
	this->meshes[name] = new Mesh(objectTextures);
	this->meshes[name]->NameMesh(modelPath.c_str());


	//Create standard material with white color
	this->materials[name] = new Material();
	this->materials[name]->fragColor = fColorRGBA{ 1, 1, 1, 1 };
	this->materials[name]->ambientValue = 0.1f;
	this->materials[name]->specularValue = 16.0f;


	//Create new rigidbody
	this->rigidBodies[name] = new Rigidbody(&this->gameObjects[name]->GetTransform(), name);

	//Push back the name to the objects vector and the gameobject to the gameobjects vector
	this->objects.push_back(name);
	this->gameObjectVector.push_back(this->gameObjects[name]);
}
//Add object pool
void Scene::AddObjectPool(std::string objectPoolName, ui32 poolSize, std::string name, std::string modelPath, bool active, std::vector<std::string> texturePaths, std::string parentName)
{
	//For size
	for (ui32 i = 0; i < poolSize; i++)
	{
		//Create new name with the template name plus the current iteration index
		std::string tempName = name;
		tempName = tempName + ' ' + std::to_string(i);

		//Create new object with that name
		AddObject(tempName, modelPath, active, texturePaths, parentName);

		//Pushback name and gameobject to dedicataed vectors/maps.
		this->objectPooledObjects[objectPoolName].push_back(tempName);
		this->gameObjects[tempName]->GetTag() = name;
	}
}
//Add component to object
void Scene::AddComponent(Component* comp, std::string name)
{
	this->components.push_back(comp);
	this->componentMap[name].push_back(comp);
}
//Add component to all objects in an object pool
void Scene::AddComponentToObjectPool(std::string name, Component* component)
{
	//For all names in a object pool
	for (std::string str : this->objectPooledObjects[name])
	{
		//get components type and add it to the object
		switch (component->GetType())
		{
		case ComponentType::Bullet:
		{
			Bullet* bullet = new Bullet(reinterpret_cast<Bullet*>(component));
			bullet->Initialize(this->rigidBodies[str]);
			AddComponent(bullet, str);
			break;
		}
		}
	}
}


//Set object to active
void Scene::SetActive(std::string objectName, bool active)
{
	if(active)
		this->objectCount++;
	else
		this->objectCount--;

	this->gameObjects[objectName]->GetIsActive() = active;
}


//Get all gameobjects in scene
std::vector<Gameobject*> Scene::GetAllGameobjects()
{
	return this->gameObjectVector;
}
//Get all components of object
std::vector<Component*> Scene::GetObjectComponents(std::string objectName)
{
	return this->componentMap[objectName];
}
//Get a object pool
std::vector<std::string> Scene::GetObjectPool(std::string poolName)
{
	return this->objectPooledObjects[poolName];
}

//Get gameobject pointer to the scene root
Gameobject* Scene::GetSceneRoot()
{
	return this->gameObjects[this->sceneName];
}
//Get specific game object
Gameobject* Scene::GetGameobject(std::string objectName)
{
	return this->gameObjects[objectName];
}
//Get specific rigidbody
Rigidbody* Scene::GetRigidBody(std::string objectName)
{
	return this->rigidBodies[objectName];
}
//Get specific component of an object
Component* Scene::GetObjectComponent(std::string objectName, ComponentType type)
{
	for (Component* comp : this->componentMap[objectName])
	{
		if (comp->GetType() == type)
			return comp;
	}

	return nullptr;
}
//Get material of object
Material* Scene::GetMaterial(std::string objectName)
{
	return this->materials[objectName];
}
//Get mesh of object
Mesh* Scene::GetMesh(std::string objectName)
{
	return this->meshes[objectName];
}
//Get scenes camera
Camera* Scene::GetCamera()
{
	return this->mainCamera;
}

//Get a first object in an objectPool
std::string Scene::GetObjectOfPool(std::string poolName)
{
	this->objectPooledObjects[poolName];

	//Get first object, delete it from the vector and then push it back to the end.
	std::string tempStr = this->objectPooledObjects[poolName][0];
	this->objectPooledObjects[poolName].erase(this->objectPooledObjects[poolName].begin());
	this->objectPooledObjects[poolName].push_back(tempStr);

	return tempStr;
}
//Get Object count
ui32 Scene::GetObjectCount()
{
	return this->objectCount;
}


//Check collision between two gameobjects
bool Scene::CheckCollision(Gameobject* gbA, Gameobject* gbB)
{
	//Get transforms
	Transform& object1 = gbA->GetTransform();
	Transform& object2 = gbB->GetTransform();

	//Compute dx, dy and use it to calculate the distance between gbA and gbB
	float dx = object1.position.x - object2.position.x;
	float dy = object1.position.y - object2.position.y;
	float distance = sqrt(dx * dx + dy * dy);

	//If the distance is smaller than both gameobjects radius added together (minus a scaling value)
	if (distance < ((gbA->GetRadius() + gbA->GetRadius()) - this->radiusReduction))
		return true; //They collide
	//Otherwise
	else
		return false; //They dont collide
}
