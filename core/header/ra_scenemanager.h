
#pragma once

//EXTERNAL INCLUDES
#include <unordered_map>
//INTERNAL INCLDUES
#include "ra_utils.h"

class Gameobject;
class Camera;

class SceneManager
{
	DEFINE_SINGLETON(SceneManager);

public:
	void Initialize();
	void Update();
	void Cleanup();

	void SwitchScene(std::string sceneName);

	Gameobject* GetActiveScene();
	Camera* GetActiveCamera();
private:
	std::unordered_map<std::string, Gameobject*> scenes;
	std::unordered_map<Gameobject*, Camera*> cameras;
	Gameobject* currentScene;
	Camera* currentCamera;
	std::string currentSceneName;

};