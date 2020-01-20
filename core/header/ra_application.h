
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_types.h"
#include <thread>
#include <string>

class Filesystem;
	
//Main class for the game.
class Application
{
	//Singleton
	DEFINE_SINGLETON(Application)

//Public functions
public:
	//Initialize Console, gamepath, filesystem, window, network, scene and renderer 
	void Initialize(const char* path, iVec2 resolution, const char* title);
	//Update each tick
	void Update();
	//Cleanup all instances and delete pointers.
	void Cleanup();


	//End the game and return to launcher
	void EndGame();
	
	
	//Set the opponents name
	void SetOpponent(std::string addr);
	

	//Get opponents name
	std::string GetOpponent();
	
	//Get Filesystem
	Filesystem* GetFilesystem();
	
	//Get deltaTime
	float GetDeltaTime();
	//Get aspectRatio
	float& GetAspectRatio();

	//Starts the game
	bool& GetEstablishState();
	//Get if this is the host
	bool& GetHostState();
	//Get running
	bool& GetRunState();
	//Get winner
	bool& GetWinnerState();

private:
	std::string opponent;
	std::thread networkThread;

	float deltaTime = 0.0f;
	float aspectRatio = 0.0f;

	Filesystem* filesystem;

	bool host;
	bool won;
	bool running = false;
	bool establishConnection;
};