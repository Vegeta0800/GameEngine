
#pragma once

//EXTERNAL INCLUDES
#include <unordered_map>
//INTERNAL INCLDUES
#include "ra_utils.h"

class Gameobject;

class SceneManager
{
	DEFINE_SINGLETON(SceneManager);

public:
	void Initialize();
	void Update();
	void Cleanup();

	void SwitchScene(std::string sceneName);

	Gameobject* GetActiveScene();
private:
	std::unordered_map<std::string, Gameobject*> scenes;
	Gameobject* currentScene;
	std::string currentSceneName;
};