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

		std::vector<Gameobject*> cubes(10000);

			float xOffset = -2.5f;
			float yOffset = 2.0f;

		for (ui32 i = 0; i < cubes.size(); i++)
		{
			std::string s = std::to_string(i);
			s = "cube " + s;
			cubes[i] = new Gameobject;

			if(i == 0)
				cubes[i]->Initialize(mainSceneRoot, s.c_str(), "cube.obj", "cubeTexture.png", true, true, true);
			else
				cubes[i]->Initialize(cubes[0], s.c_str(), "cube.obj", "cubeTexture.png", true, true, true);

			cubes[i]->GetTransform().scaling *= 0.1f;

			if (i > 0 && i % 30 == 0)
			{
				xOffset = -2.5f;
				yOffset -= 1.0f;
			}

			cubes[i]->GetTransform().position = Math::Vec3{ 0.0f, (yOffset * 5.0f) + 70.0f, (xOffset * 5.0f) - 70.0f };

			xOffset += 1.0f;
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
