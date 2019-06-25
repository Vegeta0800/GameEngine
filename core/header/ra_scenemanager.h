
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

	void SwitchScene(const char* sceneName);

	Gameobject* GetActiveScene();
private:
	std::unordered_map<const char*, Gameobject*> scenes;
	Gameobject* currentScene;
	const char* currentSceneName;
};