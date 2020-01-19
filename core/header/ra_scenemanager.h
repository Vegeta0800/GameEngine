
#pragma once
//EXTERNAL INCLUDES
#include <unordered_map>
//INTERNAL INCLDUES
#include "ra_utils.h"

//Forward declarations
class Scene;
class Camera;
class Rigidbody;
class Gameobject;
class BoxCollider;
class Mesh;
class Material;

class SceneManager
{
	//Singleton
	DEFINE_SINGLETON(SceneManager);
public:
	//Initialize Scene mangager and create the main scene
	void Initialize();
	//Update the current scene
	void Update();
	//Cleanup all scenes
	void Cleanup();


	//Switch scene
	void SwitchScene(std::string sceneName);


	//Get current scene
	Scene* GetActiveScene();
	//Get current camera
	Camera* GetActiveCamera();
private:
	std::string currentSceneName;
	std::unordered_map<std::string, Scene*> scenes;
	Scene* currentScene;
};