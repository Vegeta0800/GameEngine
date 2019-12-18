#include <string>

#include "ra_scenemanager.h"
#include "ra_gameobject.h"
#include "ra_rendering.h"
#include "ra_mesh.h"
#include "ra_camera.h"
#include "components/ra_component.h"
#include "components/player/ra_player.h"
#include "components/player/ra_bullet.h"
#include "ra_scene.h"
#include "physic/ra_rigidbody.h"

DECLARE_SINGLETON(SceneManager);

void SceneManager::Initialize()
{
	std::string mainSceneName = "";
	#pragma region MainScene
	{
		Scene* mainSceneRoot = new Scene;
		Camera* cam = new Camera;
		cam->Initialize();
		cam->GetPostion() = Math::Vec3{ 0.0f, 5.0f, 230.0f };
		cam->GetTargetOffset() = Math::Vec3{ 0.0f, 8.0f, 0.0f };

		mainSceneRoot->Initialize("MainSceneRoot", cam);
		mainSceneRoot->AddObject("Player", "player", true, std::vector<std::string>{"playerBase", "playerNormal", "playerEmission",
			"playerRoughness", "playerAmbient"});
		mainSceneRoot->AddObject("Enemy", "player", true, std::vector<std::string>{"enemyBase", "playerNormal", "playerEmission",
			"playerRoughness", "playerAmbient"});

		mainSceneRoot->GetGameobject("Player")->GetTag() = "player";
		mainSceneRoot->GetGameobject("Player")->GetTransform().scaling *= 0.2f;

		mainSceneRoot->GetGameobject("Enemy")->GetTransform().scaling *= 0.2f;
		mainSceneRoot->GetGameobject("Enemy")->GetTransform().position = Math::Vec3{ 0, 190, 0 };
		mainSceneRoot->GetGameobject("Enemy")->GetTransform().eulerRotation = Math::Vec3{ 0, 0, 180 };
		mainSceneRoot->GetGameobject("Enemy")->GetTag() = "enemy";

		mainSceneRoot->GetRigidBody("Player")->GetRigidbodyValues().gravityEnabled = false;
		mainSceneRoot->GetRigidBody("Enemy")->GetRigidbodyValues().gravityEnabled = false;

		mainSceneRoot->AddObjectPool("bullets", 1, "bullet", "shot", false);

		Bullet* bullet = new Bullet();
		bullet->Initialize(nullptr);
		bullet->GetBulletValues().activated = false;

		mainSceneRoot->AddComponentToObjectPool("bullets", bullet);

		Player* player = new Player();
		player->Initialize(mainSceneRoot->GetRigidBody("Player"));
		mainSceneRoot->AddComponent(player, "Player");

		cam->SetTarget(&mainSceneRoot->GetGameobject("Player")->GetTransform());

		mainSceneName = GetVariableName(mainSceneRoot);
		this->scenes[mainSceneName] = mainSceneRoot;
	}
	#pragma endregion

	this->currentSceneName = mainSceneName;
	this->currentScene = this->scenes[this->currentSceneName];
}

void SceneManager::SwitchScene(std::string sceneName)
{
	this->currentScene = this->scenes[sceneName];

	if (this->currentScene == nullptr)
	{
		this->currentScene = this->scenes[currentSceneName];
	}
	else
		this->currentSceneName = sceneName;
}

void SceneManager::Update()
{
	this->currentScene->Update();
}

void SceneManager::Cleanup()
{
	for (std::pair<std::string, Scene*> element : this->scenes)
	{
		element.second->Cleanup();
	}
}

Scene* SceneManager::GetActiveScene()
{
	return this->currentScene;
}

Camera* SceneManager::GetActiveCamera()
{
	return this->currentScene->GetCamera();
}
