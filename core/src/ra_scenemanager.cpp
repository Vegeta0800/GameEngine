#include <string>

#include "ra_scenemanager.h"
#include "ra_gameobject.h"
#include "ra_rendering.h"
#include "ra_mesh.h"
#include "ra_camera.h"
#include "ra_scene.h"

DECLARE_SINGLETON(SceneManager);

void SceneManager::Initialize()
{
	std::string mainSceneName = "";
	#pragma region MainScene
	{
		Scene* mainSceneRoot = new Scene;

		mainSceneRoot->Initialize("MainSceneRoot");
		mainSceneRoot->AddObject("Player", "player", std::vector<std::string>{"playerBase", "playerNormal", "playerEmission",
			"playerRoughness", "playerAmbient"});

		mainSceneRoot->Initialize("MainSceneRoot");
		mainSceneRoot->AddObject("Enemy", "player", std::vector<std::string>{"enemyBase", "playerNormal", "playerEmission",
			"playerRoughness", "playerAmbient"});

		mainSceneRoot->GetGameobject("Enemy")->GetTransform().position = Math::Vec3{ 0, 140, 0 };
		mainSceneRoot->GetGameobject("Enemy")->GetTransform().eulerRotation = Math::Vec3{ 0, 0, 180 };

		Camera* cam = new Camera;
		cam->Initialize();
		cam->GetPostion() = Math::Vec3{ 0.0f, -5.0f, 22.0f };
		cam->GetTargetOffset() = Math::Vec3{ 0.0f, 8, 0.0f };
		cam->SetTarget(&mainSceneRoot->GetGameobject("Player")->GetTransform());

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
	for (std::pair<std::string, Scene*> element : this->scenes)
	{
		element.second->Cleanup();
	}

	for (std::pair<Scene*, Camera*> element : this->cameras)
	{
		delete element.second;
	}
}

Scene* SceneManager::GetActiveScene()
{
	return this->currentScene;
}

Camera* SceneManager::GetActiveCamera()
{
	return this->currentCamera;
}
