#include <string>

#include "ra_scenemanager.h"
#include "ra_gameobject.h"
#include "ra_rendering.h"
#include "ra_mesh.h"
#include "ra_camera.h"


DECLARE_SINGLETON(SceneManager);

void SceneManager::Initialize()
{
	std::string renderingExampleScene = "";
	std::string mainSceneName = "";
	#pragma region RenderingExampleScene
	{
		Gameobject* renderingSceneRoot = new Gameobject;
		renderingSceneRoot->MakeRoot();
		renderingSceneRoot->SetName("RenderingSceneRoot");

		std::vector<Gameobject*> cubes(1000);

			float xOffset = -2.5f;
			float yOffset = 2.0f;

		for (ui32 i = 0; i < cubes.size(); i++)
		{
			std::string s = std::to_string(i);
			s = "cube " + s;
			cubes[i] = new Gameobject;

			if(i == 0)
				cubes[i]->Initialize(renderingSceneRoot, s.c_str(), "cube.obj", std::vector<const char*>{"cubeTexture.png"}, true, true, true);
			else
				cubes[i]->Initialize(cubes[0], s.c_str(), "cube.obj", std::vector<const char*>{"cubeTexture.png"}, true, true, true);

			cubes[i]->GetTransform().scaling *= 0.1f;

			if (i > 0 && i % 30 == 0)
			{
				xOffset = -2.5f;
				yOffset -= 1.0f;
			}

			cubes[i]->GetTransform().position = Math::Vec3{ (yOffset * 5.0f) + 70.0f, 0.0f, (xOffset * 5.0f) - 70.0f };

			xOffset += 1.0f;
		}

		Camera* cam = new Camera;
		cam->Initialize();
		cam->GetPostion() = Math::Vec3{ 0.0f, -20.0f, -1.0f };
		

		renderingExampleScene = GetVariableName(renderingSceneRoot);
		this->scenes[renderingExampleScene] = renderingSceneRoot;
		this->cameras[renderingSceneRoot] = cam;
	}
	#pragma endregion

	#pragma region MainScene
	{
		Gameobject* mainSceneRoot = new Gameobject;
		mainSceneRoot->MakeRoot();
		mainSceneRoot->SetName("MainSceneRoot");

		Gameobject* player = new Gameobject;
		player->Initialize(mainSceneRoot, "Player", "player.obj", 
			std::vector<const char*>{"playerBase.png", "playerNormal.png", "playerEmission.png", 
			"playerRoughness.png", "playerAmbient.png"}, true, true, false);
		player->GetTransform().scaling *= 0.3f;
		player->GetTransform().position = Math::Vec3{0, 0, 0};

		Gameobject* player2 = new Gameobject;
		player2->Initialize(mainSceneRoot, "Player2", "player.obj",
			std::vector<const char*>{"playerBase.png", "playerNormal.png", "playerEmission.png",
			"playerRoughness.png", "playerAmbient.png"}, true, true, false);
		player2->GetTransform().scaling *= 0.3f;
		player2->GetTransform().position = Math::Vec3{ 0, -100, 0 };
		player2->GetTransform().eulerRotation = Math::Vec3{ 0, 0, 180 };
		player2->GetMaterial().fragColor = fColorRGBA{1, 0, 0, 1};

		Camera* cam = new Camera;
		cam->Initialize();
		cam->GetPostion() = Math::Vec3{ 0.0f, -1.0f, 22.0f };
		cam->GetTargetOffset() = Math::Vec3{ 0.0f, 5, 0.0f };
		cam->SetTarget(player);

		mainSceneName = GetVariableName(mainSceneRoot);
		this->scenes[mainSceneName] = mainSceneRoot;
		this->cameras[mainSceneRoot] = cam;
	}
	#pragma endregion

	this->currentSceneName = mainSceneName;
	this->currentScene = this->scenes[this->currentSceneName];
	this->currentCamera = this->cameras[this->currentScene];
}

void SceneManager::SwitchScene(std::string sceneName)
{
	this->currentScene = this->scenes[sceneName];

	if (this->currentScene == nullptr)
	{
		this->currentScene = this->scenes[currentSceneName];
		this->currentCamera = this->cameras[this->currentScene];
	}
	else
		this->currentSceneName = sceneName;
}

void SceneManager::Update()
{
	this->currentScene->Update();
	this->currentCamera->Update();
}

void SceneManager::Cleanup()
{
	for (std::pair<std::string, Gameobject*> element : this->scenes)
	{
		element.second->Cleanup();
	}

	for (std::pair<Gameobject*, Camera*> element : this->cameras)
	{
		delete element.second;
	}
}

Gameobject* SceneManager::GetActiveScene()
{
	return this->currentScene;
}

Camera* SceneManager::GetActiveCamera()
{
	return this->currentCamera;
}
