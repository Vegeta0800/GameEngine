#include "ra_scenemanager.h"
#include "ra_gameobject.h"

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
		testObject->Initialize(MeshType::MESH_DRAGON, mainSceneRoot, GetVariableName(testObject));
		testObject->GetTransform().scaling *= 0.1f;
		testObject->GetTransform().position.z -= 1.0f;
		//testObject->SetMeshColor(Math::Vec3{ 0.2f, 0.349f, 0.964f });

		Gameobject* testObject2 = new Gameobject;
		testObject2->Initialize(testObject, GetVariableName(testObject2));

		//testObject2->SetMeshColor(Math::Vec3{ 0.874f, 0.749f, 0.164f });

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
