
//EXTERNAL INCLUDES
#include <thread>
#include <ctime>
#include <chrono>
//INTERNAL INCLUDES
#include "ra_application.h"
#include "console/ra_cvar.h"
#include "console/ra_console.h"
#include "input/ra_inputhandler.h"
#include "ra_utils.h"
#include "ra_window.h"
#include "ra_rendering.h"
#include "ra_scenemanager.h"
#include "ra_gameobject.h"
#include "ra_network.h"

//Start network as either host or client
void NetworkStart(bool host)
{
	Network* network = new Network();

	if (host)
		network->InitializeHost();
	else
		network->InitializeClient();

	delete network;
}

//Make application a singleton
DECLARE_SINGLETON(Application)

//Initialize Console, gamepath, filesystem, window, network, scene and renderer 
void Application::Initialize(const char* path, iVec2 resolution, const char* title)
{
	//Swap \\ to / so data can get read from files. 
	std::string gamepathstr = static_cast<std::string>(path);
	for (char& sign : gamepathstr)
	{
		if (sign == '\\')
		{
			sign = '/';
		}
	}
	//Set gamepath as a cvar
	gamepathstr = gamepathstr.substr(0, gamepathstr.find_last_of("/") + 1);
	CVar gamepath = { "gamepath", gamepathstr.c_str() };
	Console::GetInstancePtr()->SetCVar(gamepath);

	//Create filesystem and instantiate window
	this->filesystem = new Filesystem;
	Window::GetInstancePtr()->Instantiate(800, 600, 0, "windowTest");

	//Initialize the SceneManager
	SceneManager::GetInstancePtr()->Initialize();
	
	//Run
	this->running = true;

	//Start network on a new thread and detach it.
	this->networkThread = std::thread(std::bind(NetworkStart, this->host));
	this->networkThread.detach();

	//Initialize rendering
	Rendering::GetInstancePtr()->Initialize("RenderingExample", VK_MAKE_VERSION(0, 0, 0));
}
//Update each tick
void Application::Update()
{
	INIT_TIMER

	//Start deltaTime
	this->deltaTime = 0.0f;

	auto gameStartTime = std::chrono::high_resolution_clock::now();
	auto gameCurrentTime = std::chrono::high_resolution_clock::now();

	//While window opened
	while (Window::GetInstancePtr() && Window::GetInstancePtr()->GetState() != Window::WindowState::Closed)
	{
		this->running = true;

		START_TIMER
		//Update deltaTimex
		gameStartTime = std::chrono::high_resolution_clock::now();
		this->deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(gameStartTime - gameCurrentTime).count();
		gameCurrentTime = gameStartTime;
		  
		//And if there are no new window events
		if (!Window::GetInstancePtr()->PollEvents())
		{
			//Update Scene and renderer
			SceneManager::GetInstancePtr()->Update();
			Rendering::GetInstancePtr()->Update();

			//Delete keys in upstate
			if (Input::GetInstancePtr()->GetUpState())
				Input::GetInstancePtr()->EradicateUpKeys();

		}
		STOP_TIMER("Loop took")
	}

	//Stop app
	this->running = false;
}
//Cleanup all instances and delete pointers.
void Application::Cleanup()
{
	Rendering::GetInstancePtr()->Cleanup();
	SceneManager::GetInstancePtr()->Cleanup();
	this->filesystem->Cleanup();
	Window::GetInstancePtr()->Release();
	Console::GetInstancePtr()->Cleanup();
	Console::GetInstancePtr()->Release();
	Rendering::GetInstancePtr()->Release();
	SceneManager::GetInstancePtr()->Release();
	Input::GetInstancePtr()->Release();
}


//End the game and return to launcher
void Application::EndGame()
{
	Window::GetInstancePtr()->SetState(Window::WindowState::Closed);
}


//Set the opponents name
void Application::SetOpponent(std::string opp)
{
	this->opponent = opp;
}

//Get opponents name
std::string Application::GetOpponent()
{
	return this->opponent;
}

//Get Filesystem
Filesystem* Application::GetFilesystem()
{
	return this->filesystem;
}

//Get deltaTime
float Application::GetDeltaTime()
{
	return this->deltaTime;
}
//Get aspectRatio
float& Application::GetAspectRatio()
{
	return this->aspectRatio;
}

//Starts the game
bool& Application::GetEstablishState()
{
	return this->establishConnection;
}
//Get if this is the host
bool& Application::GetHostState()
{
	return this->host;
}
//Get running
bool& Application::GetRunState()
{
	return this->running;
}
//Get winner
bool& Application::GetWinnerState()
{
	return this->won;
}
