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
						bool b = CheckCollision(this->gameObjectVector[i], this->gameObjectVector[j]);

						if (b)
						{
							printf("COLLIDING");
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
	Math::Mat4x4 aTemp = this->mainCamera->GetVPMatrix() *
		Math::CreateModelMatrix((gbA->GetTransform().position), gbA->GetTransform().scaling, Math::Vec3::zero);

	Math::Mat4x4 bTemp = this->mainCamera->GetVPMatrix() *
		Math::CreateModelMatrix((gbB->GetTransform().position), gbB->GetTransform().scaling, Math::Vec3::zero);

	std::string aString = gbA->GetName();
	std::string bString = gbB->GetName();

	Vec4 aCorners4[4] =
	{
		Vec4{this->boxCollider[aString]->GetMax().x , this->boxCollider[aString]->GetMax().y, 0.0f, 1.0f} * aTemp,
		Vec4{this->boxCollider[aString]->GetMin().x , this->boxCollider[aString]->GetMax().y, 0.0f, 1.0f} * aTemp,
		Vec4{this->boxCollider[aString]->GetMin().x , this->boxCollider[aString]->GetMin().y, 0.0f, 1.0f} * aTemp,
		Vec4{this->boxCollider[aString]->GetMax().x , this->boxCollider[aString]->GetMin().y, 0.0f, 1.0f} * aTemp,
	};																									    
																										    
	Vec4 bCorners4[4] =																					    
	{																									    
		Vec4{this->boxCollider[bString]->GetMax().x , this->boxCollider[bString]->GetMax().y, 0.0f, 1.0f} * bTemp,
		Vec4{this->boxCollider[bString]->GetMin().x , this->boxCollider[bString]->GetMax().y, 0.0f, 1.0f} * bTemp,
		Vec4{this->boxCollider[bString]->GetMin().x , this->boxCollider[bString]->GetMin().y, 0.0f, 1.0f} * bTemp,
		Vec4{this->boxCollider[bString]->GetMax().x , this->boxCollider[bString]->GetMin().y, 0.0f, 1.0f} * bTemp,
	};

	Math::Vec3 aCorners[4] =
	{
		Math::Vec3{ aCorners4[0].r, aCorners4[0].g, 0.0f},
		Math::Vec3{ aCorners4[1].r, aCorners4[1].g, 0.0f},
		Math::Vec3{ aCorners4[2].r, aCorners4[2].g, 0.0f},
		Math::Vec3{ aCorners4[3].r, aCorners4[3].g, 0.0f},
	};

	Math::Vec3 bCorners[4] =
	{
		Math::Vec3{ bCorners4[0].r, bCorners4[0].g, 0.0f},
		Math::Vec3{ bCorners4[1].r, bCorners4[1].g, 0.0f},
		Math::Vec3{ bCorners4[2].r, bCorners4[2].g, 0.0f},
		Math::Vec3{ bCorners4[3].r, bCorners4[3].g, 0.0f},
	};

	Math::Vec3 axis1 = GetAxis(aCorners[0], aCorners[1]);
	if (!isProjectionIntersecting(aCorners, bCorners, axis1)) return false;

	Math::Vec3 axis2 = GetAxis(aCorners[0], aCorners[3]);
	if (!isProjectionIntersecting(aCorners, bCorners, axis2)) return false;

	Math::Vec3 axis3 = GetAxis(bCorners[1], bCorners[2]);
	if (!isProjectionIntersecting(aCorners, bCorners, axis3)) return false;

	Math::Vec3 axis4 = GetAxis(bCorners[1], bCorners[0]);
	if (!isProjectionIntersecting(aCorners, bCorners, axis4)) return false;

	return true;
}