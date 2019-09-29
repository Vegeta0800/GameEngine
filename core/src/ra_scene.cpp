#include <unordered_set>

#include "ra_scene.h"
#include "ra_camera.h"
#include "ra_gameobject.h"
#include "ra_application.h"
#include "filesystem/ra_filesystem.h"
#include "ra_mesh.h"
#include "ra_texture.h"
#include "ra_material.h"
#include "physic/ra_rigidbody.h"
#include "ra_boxcollider.h"
#include "math/ra_mathfunctions.h"
#include "input/ra_inputhandler.h"
#include "components/player/ra_player.h"
#include "components/player/ra_bullet.h"

void Scene::Initialize(std::string name, Camera* camera)
{
	this->sceneName = name;
	Gameobject* scene = new Gameobject;
	scene->MakeRoot();
	scene->SetName(name);

	this->gameObjects[name] = scene;

	if (camera == nullptr)
	{
		this->mainCamera = new Camera;
		this->mainCamera->Initialize();
	}
	else
		this->mainCamera = camera;
}

void Scene::Update()
{
	UpdateCollisions();

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
		}
	}

	for (std::string object : this->objects)
	{
		if(this->gameObjects[object]->GetIsActive())
			this->rigidBodies[object]->Update();
	}

	this->gameObjects[this->sceneName]->Update();

	this->mainCamera->Update();

}

void Scene::UpdateCollisions()
{
	std::unordered_set<Gameobject*> checkedGameobjects;

	for (int i = 0; i < this->gameObjectVector.size(); i++)
	{
		if (this->boxCollider[this->gameObjectVector[i]->GetName()]->hasCollision() 
			&& this->gameObjectVector[i]->GetIsActive())
		{
			for (int j = 0; j < this->gameObjectVector.size(); j++)
			{
				if (j != i && this->gameObjectVector[j]->GetIsActive()
					&& this->boxCollider[this->gameObjectVector[j]->GetName()]->hasCollision()
					&& this->gameObjectVector[i]->GetTag() != this->gameObjectVector[j]->GetTag())
				{
					if (checkedGameobjects.find(this->gameObjectVector[j]) == checkedGameobjects.end())
					{
						if (CheckCollision(this->gameObjectVector[i], this->gameObjectVector[j]))
						{
							this->rigidBodies[this->gameObjectVector[i]->GetName()]->GetRigidbodyValues().isColliding = true;
							this->rigidBodies[this->gameObjectVector[j]->GetName()]->GetRigidbodyValues().isColliding = true;

							this->rigidBodies[this->gameObjectVector[i]->GetName()]
								->SetHitObject(this->rigidBodies[this->gameObjectVector[j]->GetName()]);
							this->rigidBodies[this->gameObjectVector[j]->GetName()]
								->SetHitObject(this->rigidBodies[this->gameObjectVector[i]->GetName()]);
						}
						else
						{
							this->rigidBodies[this->gameObjectVector[i]->GetName()]->GetRigidbodyValues().isColliding = false;
							this->rigidBodies[this->gameObjectVector[j]->GetName()]->GetRigidbodyValues().isColliding = false;

							this->rigidBodies[this->gameObjectVector[i]->GetName()]->SetHitObject(nullptr);
							this->rigidBodies[this->gameObjectVector[j]->GetName()]->SetHitObject(nullptr);
						}
					}
				}
			}

			checkedGameobjects.insert(this->gameObjectVector[i]);
		}
	}
}

void Scene::Cleanup()
{
	for (Component* comp : this->components)
	{
		switch (comp->GetType())
		{
		case ComponentType::Player:
			reinterpret_cast<Player*>(comp)->Cleanup();
		}
	}

	for (std::string object : this->objects)
	{
		this->rigidBodies[object]->Cleanup();
		this->meshes[object]->Cleanup();
	}

	delete this->mainCamera;

	this->gameObjects[this->sceneName]->Cleanup();

	this->gameObjects.clear();
	this->meshes.clear();
	this->materials.clear();
	this->boxCollider.clear();
	this->rigidBodies.clear();
}

ui32 Scene::GetObjectCount()
{
	return this->objectCount;
}

Gameobject* Scene::GetSceneRoot()
{
	return this->gameObjects[this->sceneName];
}

Gameobject* Scene::GetGameobject(std::string objectName)
{
	return this->gameObjects[objectName];
}

std::vector<Gameobject*> Scene::GetAllGameobjects()
{
	return this->gameObjectVector;
}

std::vector<std::string> Scene::GetObjectPool(std::string poolName)
{
	return this->objectPooledObjects[poolName];
}

std::string Scene::GetObjectOfPool(std::string poolName)
{
	this->objectPooledObjects[poolName];

	std::string tempStr = this->objectPooledObjects[poolName][0];
	this->objectPooledObjects[poolName].erase(this->objectPooledObjects[poolName].begin());
	this->objectPooledObjects[poolName].push_back(tempStr);

	return tempStr;
}

Material* Scene::GetMaterial(std::string objectName)
{
	return this->materials[objectName];
}

Mesh* Scene::GetMesh(std::string objectName)
{
	return this->meshes[objectName];
}

Camera* Scene::GetCamera()
{
	return this->mainCamera;
}

Rigidbody* Scene::GetRigidBody(std::string objectName)
{
	return this->rigidBodies[objectName];
}

BoxCollider* Scene::GetBoxCollider(std::string objectName)
{
	return this->boxCollider[objectName];
}

Component* Scene::GetObjectComponent(std::string objectName)
{
	return this->componentMap[objectName];
}

void Scene::AddObject(std::string name, std::string modelPath, bool active, std::vector<std::string> texturePaths, std::string parentName)
{
	if (active)
		this->objectCount++;

	//Gameobject
	this->gameObjects[name] = new Gameobject();
	if (parentName != "")
		this->gameObjects[name]->Initialize(this->gameObjects[parentName], name, active);
	else
		this->gameObjects[name]->Initialize(this->gameObjects[this->sceneName], name, active);

	//Textures
	std::vector<Texture*> objectTextures(5);
	std::string path = Application::GetInstancePtr()->GetFilesystem()->DirectoryPath("textures");

	path.pop_back();

	if (texturePaths.size() == 0) 
	{
		for (int i = 0; i < objectTextures.size(); i++)
		{
			Texture* tex = new Texture();
			tex->GetPath() = path + "empty.png";
			objectTextures[i] = tex;
		}
	}
	else
	{
		for (int i = 0; i < texturePaths.size(); i++)
		{
			Texture* tex = new Texture();
			texturePaths[i] = path + texturePaths[i] + ".png";
			tex->GetPath() = texturePaths[i];
			objectTextures[i] = tex;
		}
	}

	modelPath = modelPath + ".obj";

	//Mesh
	this->meshes[name] = new Mesh(objectTextures);
	this->meshes[name]->CreateMesh(modelPath.c_str());

	//Material
	this->materials[name] = new Material();
	this->materials[name]->fragColor = fColorRGBA{ 1, 1, 1, 1 };
	this->materials[name]->ambientValue = 0.1f;
	this->materials[name]->specularValue = 16.0f;

	//Rigidbody
	this->rigidBodies[name] = new Rigidbody(&this->gameObjects[name]->GetTransform(), name);

	//BoxColliders
	this->boxCollider[name] = new BoxCollider(&this->gameObjects[name]->GetTransform());
	this->boxCollider[name]->hasCollision() = true;


	this->objects.push_back(name);
	this->gameObjectVector.push_back(this->gameObjects[name]);
}

void Scene::AddComponentToObjectPool(std::string name, Component* component)
{
	for (std::string str : this->objectPooledObjects[name])
	{
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

void Scene::AddObjectPool(std::string objectPoolName, ui32 poolSize, std::string name, std::string modelPath, bool active, std::vector<std::string> texturePaths, std::string parentName)
{
	for (ui32 i = 0; i < poolSize; i++)
	{
		std::string tempName = name;
		tempName = tempName + ' ' + std::to_string(i);
		AddObject(tempName, modelPath, active, texturePaths, parentName);
		this->objectPooledObjects[objectPoolName].push_back(tempName);
		this->gameObjects[tempName]->GetTag() = name;

	}
}

void Scene::AddComponent(Component* comp, std::string name)
{
	this->components.push_back(comp);
	this->componentMap[name] = comp;
}

void Scene::SetActive(std::string objectName, bool active)
{
	if(active)
		this->objectCount++;
	else
		this->objectCount--;

	this->gameObjects[objectName]->GetIsActive() = active;
}

Math::Vec3 Scene::GetAxis(Math::Vec3 point1, Math::Vec3 point2)
{
	Math::Vec3 edge = point1 - point2;
	Math::Vec3 edgeNormal = Math::Vec3{ -edge.y, edge.x, edge.z };
	Math::Normalize(edgeNormal);
	return edgeNormal;
}

bool Scene::isProjectionIntersecting(Math::Vec3 aCorners[], Math::Vec3 bCorners[], Math::Vec3 axis)
{
	float aMin, aMax, bMin, bMax = 0.0f;

	Math::GetMinMaxOfProjection(aCorners, axis, aMin, aMax);
	Math::GetMinMaxOfProjection(bCorners, axis, bMin, bMax);

	if (aMin > bMax) return false;
	if (aMax < bMin) return false;

	return true;
}

bool Scene::CheckCollision(Gameobject* gbA, Gameobject* gbB)
{
	std::string aString = gbA->GetName();
	std::string bString = gbB->GetName();

	Transform object1 = gbA->GetTransform();
	Transform object2 = gbB->GetTransform();

	return (object1.position.x < object2.position.x + this->boxCollider[bString]->GetWidth() &&
		object1.position.x + this->boxCollider[aString]->GetWidth() > object2.position.x &&
		object1.position.y < object2.position.y + this->boxCollider[bString]->GetHeight() &&
		object1.position.y + this->boxCollider[aString]->GetHeight() > object2.position.y);
}