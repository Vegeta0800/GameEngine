#include <string>

#include "ra_scenemanager.h"
#include "ra_gameobject.h"
#include "ra_rendering.h"
#include "ra_mesh.h"

DECLARE_SINGLETON(SceneManager);

void SceneManager::Initialize()
{
	const char* mainSceneName = "";
	#pragma region MainScene
	{
		Gameobject* mainSceneRoot = new Gameobject;
		mainSceneRoot->MakeRoot();
		mainSceneRoot->SetName("MainSceneRoot");

		Gameobject* testObject = new Gameobject;
		testObject->Initialize(mainSceneRoot, GetVariableName(testObject), "dragon.obj", "texture.jpg", true, true, false);
		testObject->GetTransform().scaling *= 0.1f;
		testObject->GetTransform().position = Math::Vec3{ 0, 0, 0 };

		std::vector<Gameobject*> cubes(500);

		for (ui32 i = 0; i < cubes.size(); i++)
		{
			std::string s = std::to_string(i);
			s = "cube " + s;
			cubes[i] = new Gameobject;
			cubes[i]->Initialize(mainSceneRoot, s.c_str(), "dragon.obj", "texture.jpg", true, true, true);
			cubes[i]->GetTransform().scaling *= 0.1f;
			cubes[i]->GetTransform().position = Math::Vec3{ 0, 5.0f * i, 0 };
			cubes[i]->GetMaterial().fragColor = fColorRGBA{ 0.874f, 0.749f, 0.164f, 1.0f };
		}
		

		mainSceneName = GetVariableName(mainSceneRoot);
		this->scenes[mainSceneName] = mainSceneRoot;
	}
	#pragma endregion

	this->currentSceneName = mainSceneName;
	this->currentScene = this->scenes[mainSceneName];
}

void SceneManager::SwitchScene(const char* sceneName)
{
	this->currentScene = this->scenes[sceneName];

	if (this->currentScene == nullptr)
		this->currentScene = this->scenes[currentSceneName];
	else
		this->currentSceneName = sceneName;
}

void SceneManager::Update()
{
	this->currentScene->Update();
}

void SceneManager::Cleanup()
{
	for (std::pair<const char*, Gameobject*> element : this->scenes)
	{
		element.second->Cleanup();
	}
}

Gameobject* SceneManager::GetActiveScene()
{
	return this->currentScene;
}
