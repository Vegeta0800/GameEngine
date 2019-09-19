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
		}
	}

	for (std::string object : this->objects)
	{
		this->rigidBodies[object]->Update();
	}

	this->gameObjects[this->sceneName]->Update();

	this->mainCamera->Update();

}

void Scene::UpdateCollisions()
{
	std::unordered_set<Gameobject*> checkedGameobjects;

	if (Input::GetInstancePtr()->GetKeyDown(KeyCode::B))
	{
		bool be = true;
	}

	for (int i = 0; i < this->gameObjectVector.size(); i++)
	{
		if (this->boxCollider[this->gameObjectVector[i]->GetName()]->hasCollision())
		{
			for (int j = 0; j < this->gameObjectVector.size(); j++)
			{
				if (j != i)
				{
					if (checkedGameobjects.find(this->gameObjectVector[j]) == checkedGameobjects.end())
					{
						if (CheckCollision(this->gameObjectVector[i], this->gameObjectVector[j]))
						{
							this->rigidBodies[this->gameObjectVector[i]->GetName()]->GetRigidbodyValues().isColliding = true;
							this->rigidBodies[this->gameObjectVector[j]->GetName()]->GetRigidbodyValues().isColliding = true;
						}
						else
						{
							this->rigidBodies[this->gameObjectVector[i]->GetName()]->GetRigidbodyValues().isColliding = false;
							this->rigidBodies[this->gameObjectVector[j]->GetName()]->GetRigidbodyValues().isColliding = false;
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

void Scene::AddObject(std::string name, std::string modelPath, std::vector<std::string> texturePaths, std::string parentName)
{
	//Gameobject
	this->gameObjects[name] = new Gameobject();
	if (parentName != "")
		this->gameObjects[name]->Initialize(this->gameObjects[parentName], name);
	else
		this->gameObjects[name]->Initialize(this->gameObjects[this->sceneName], name);

	//Textures
	std::vector<Texture*> objectTextures(5);
	std::string path = Application::GetInstancePtr()->GetFilesystem()->DirectoryPath("textures");

	path.pop_back();

	for(int i = 0; i < texturePaths.size(); i++)
	{
		Texture* tex = new Texture();
		texturePaths[i] = path + texturePaths[i] + ".png";
		tex->GetPath() = texturePaths[i];
		objectTextures[i] = tex;
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
	this->rigidBodies[name] = new Rigidbody(&this->gameObjects[name]->GetTransform());

	//BoxColliders
	this->boxCollider[name] = new BoxCollider(&this->gameObjects[name]->GetTransform());
	this->boxCollider[name]->hasCollision() = true;


	this->objects.push_back(name);
	this->gameObjectVector.push_back(this->gameObjects[name]);
}

void Scene::AddComponent(Component* comp)
{
	this->components.push_back(comp);
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

	float object1Width = Math::Distance(
		Math::Vec3{ this->boxCollider[aString]->GetMinMax()[1].x, this->boxCollider[aString]->GetMinMax()[1].y, 0 },
		Math::Vec3{ this->boxCollider[aString]->GetMinMax()[0].x, this->boxCollider[aString]->GetMinMax()[1].y, 0}
	);

	float object2Width = Math::Distance(
			Math::Vec3{ this->boxCollider[aString]->GetMinMax()[1].x, this->boxCollider[aString]->GetMinMax()[1].y, 0 },
			Math::Vec3{ this->boxCollider[aString]->GetMinMax()[0].x, this->boxCollider[aString]->GetMinMax()[1].y, 0 }
	);

	float object1Height = Math::Distance(
		Math::Vec3{ this->boxCollider[bString]->GetMinMax()[1].x, this->boxCollider[bString]->GetMinMax()[1].y, 0 },
		Math::Vec3{ this->boxCollider[bString]->GetMinMax()[1].x, this->boxCollider[bString]->GetMinMax()[0].y, 0 }
	);

	float object2Height = Math::Distance(
		Math::Vec3{ this->boxCollider[bString]->GetMinMax()[1].x, this->boxCollider[bString]->GetMinMax()[1].y, 0 },
		Math::Vec3{ this->boxCollider[bString]->GetMinMax()[1].x, this->boxCollider[bString]->GetMinMax()[0].y, 0 }
	);

	return (object1.position.x < object2.position.x + object2Width &&
		object1.position.x + object1Width > object2.position.x &&
		object1.position.y < object2.position.y + object2Height &&
		object1.position.y + object1Height > object2.position.y);
}