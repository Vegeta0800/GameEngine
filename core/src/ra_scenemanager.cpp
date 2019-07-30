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

		Gameobject* testObject2 = new Gameobject;
		testObject2->Initialize(mainSceneRoot, GetVariableName(testObject2), "dragon(2).obj", "texture(2).jpg", true, true, true);
		testObject2->GetTransform().scaling *= 0.1f;
		testObject2->GetTransform().position = Math::Vec3{ 0, -15, 0 };
		testObject2->GetMaterial().fragColor = fColorRGBA{ 0.874f, 0.749f, 0.164f, 1.0f };

		Gameobject* testObject3 = new Gameobject;
		testObject3->Initialize(mainSceneRoot, GetVariableName(testObject3), "dragon(2).obj", "texture(2).jpg", true, true, true);
		testObject3->GetTransform().scaling *= 0.1f;
		testObject3->GetTransform().position = Math::Vec3{ 0, 15, 0 };
		testObject3->GetMaterial().fragColor = fColorRGBA{ 0.2f, 0.349f, 0.964f, 1.0f };

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
