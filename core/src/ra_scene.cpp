#include "ra_scene.h"
#include "ra_gameobject.h"
#include "ra_application.h"
#include "filesystem/ra_filesystem.h"
#include "ra_mesh.h"
#include "ra_texture.h"
#include "ra_material.h"
#include "physic/ra_rigidbody.h"
#include "ra_boxcollider.h"

void Scene::Initialize(std::string name)
{
	this->sceneName = name;
	Gameobject* scene = new Gameobject;
	scene->MakeRoot();
	scene->SetName(name);

	this->gameObjects[name] = scene;
}

void Scene::Update()
{
	for (std::string object : this->objects)
	{
		this->rigidBodies[object]->Update();
	}

	this->gameObjects[this->sceneName]->Update();
}

void Scene::Cleanup()
{
	for (std::string object : this->objects)
	{
		this->rigidBodies[object]->Cleanup();
		this->meshes[object]->Cleanup();
	}

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

Material* Scene::GetMaterial(std::string objectName)
{
	return this->materials[objectName];
}

Mesh* Scene::GetMesh(std::string objectName)
{
	return this->meshes[objectName];
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

	this->objects.push_back(name);
}