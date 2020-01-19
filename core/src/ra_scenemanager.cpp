
//EXTERNAL INCLUDES
#include <string>
//INTERNAL INCLUDES
#include "ra_scenemanager.h"
#include "ra_gameobject.h"
#include "ra_rendering.h"
#include "ra_mesh.h"
#include "ra_camera.h"
#include "components/ra_component.h"
#include "components/player/ra_player.h"
#include "components/player/ra_bullet.h"
#include "components/enemy/ra_enemy.h"
#include "ra_scene.h"
#include "physic/ra_rigidbody.h"

//Singleton
DECLARE_SINGLETON(SceneManager);

//Initialize Scene mangager and create the main scene
void SceneManager::Initialize()
{
	//Create main scene
	std::string mainSceneName = "";
	#pragma region MainScene
	{
		//Create scene
		Scene* mainSceneRoot = new Scene;
		
		//Create main camera		
		Camera* cam = new Camera;
		cam->Initialize();
		cam->GetPostion() = Math::Vec3{ 0.0f, 5.0f, 230.0f };
		cam->GetTargetOffset() = Math::Vec3{ 0.0f, 8.0f, 0.0f };
		
		//Initialize Scene with camera
		mainSceneRoot->Initialize("MainSceneRoot", cam);

		//Add Objects to scene
		mainSceneRoot->AddObject("Player", "player", true, std::vector<std::string>{"playerBase", "playerNormal", "playerEmission",
			"playerRoughness", "playerAmbient"});
		mainSceneRoot->AddObject("Enemy", "player", true, std::vector<std::string>{"enemyBase", "playerNormal", "playerEmission",
			"playerRoughness", "playerAmbient"});

		//Set settings for player object
		mainSceneRoot->GetGameobject("Player")->GetTag() = "player";
		mainSceneRoot->GetGameobject("Player")->GetTransform().scaling *= 0.2f;

		//Set settings for enemy object
		mainSceneRoot->GetGameobject("Enemy")->GetTransform().scaling *= 0.2f;
		mainSceneRoot->GetGameobject("Enemy")->GetTransform().position = Math::Vec3{ 0, 190, 0 };
		mainSceneRoot->GetGameobject("Enemy")->GetTransform().eulerRotation = Math::Vec3{ 0, 0, 180 };
		mainSceneRoot->GetGameobject("Enemy")->GetTag() = "enemy";

		//Set physics settings for objects
		mainSceneRoot->GetRigidBody("Player")->GetRigidbodyValues().gravityEnabled = false;
		mainSceneRoot->GetRigidBody("Enemy")->GetRigidbodyValues().gravityEnabled = false;

		//Create bullet object pool
		mainSceneRoot->AddObjectPool("bullets", 5, "bullet", "shot", false);

		//Create bullet blueprint component and initialize it
		Bullet* bullet = new Bullet();
		bullet->Initialize(nullptr);
		bullet->GetBulletValues().activated = false;

		//Add bullet to all objects in bullet object pool
		mainSceneRoot->AddComponentToObjectPool("bullets", bullet);

		//Create player component, initialize and add it to the player object
		Player* player = new Player();
		player->Initialize(mainSceneRoot->GetRigidBody("Player"));
		mainSceneRoot->AddComponent(player, "Player");

		//Create enemy component, initialize and add it to the enemy object
		Enemy* enemy = new Enemy();
		enemy->Initialize(mainSceneRoot->GetRigidBody("Enemy"));
		mainSceneRoot->AddComponent(enemy, "Enemy");

		//Set camera target to player
		cam->SetTarget(&mainSceneRoot->GetGameobject("Player")->GetTransform());

		//Set scene name string to variable name
		mainSceneName = GetVariableName(mainSceneRoot);
		//Add scene to scenes map
		this->scenes[mainSceneName] = mainSceneRoot;
	}
	#pragma endregion

	//Set current scene and name to main scene
	this->currentSceneName = mainSceneName;
	this->currentScene = this->scenes[this->currentSceneName];
}
//Update the current scene
void SceneManager::Update()
{
	this->currentScene->Update();
}
//Cleanup all scenes
void SceneManager::Cleanup()
{
	for (std::pair<std::string, Scene*> element : this->scenes)
	{
		element.second->Cleanup();
		delete element.second;
	}

	this->scenes.clear();
	this->currentScene = nullptr;
}


//Switch scene
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


//Get current scene
Scene* SceneManager::GetActiveScene()
{
	return this->currentScene;
}
//Get current camera
Camera* SceneManager::GetActiveCamera()
{
	return this->currentScene->GetCamera();
}
