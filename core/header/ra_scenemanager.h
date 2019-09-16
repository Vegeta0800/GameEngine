
#pragma once

//EXTERNAL INCLUDES
#include <unordered_map>
//INTERNAL INCLDUES
#include "ra_utils.h"

class Scene;
class Camera;

class Rigidbody;
class Gameobject;
class BoxCollider;
class Mesh;
class Material;

class SceneManager
{
	DEFINE_SINGLETON(SceneManager);

public:
	void Initialize();
	void Update();
	void Cleanup();

	void SwitchScene(std::string sceneName);

	Scene* GetActiveScene();
	Camera* GetActiveCamera();

private:
	std::unordered_map<std::string, Scene*> scenes;
	Scene* currentScene;
	std::string currentSceneName;

};