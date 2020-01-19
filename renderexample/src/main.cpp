
//EXTERNAL INCLUDES
#include <thread>
//INTERNAL INCLUDES
#include "ra_application.h"
#include "ra_launcher.h"
#include "ra_utils.h"
#include "ra_types.h"

//Start game
bool startGame = false;

//Launcher thread function
void ExecuteLauncher()
{
	Launcher launcher = Launcher();

	startGame = launcher.Startup();
}

int main(int argc, const char* argv[])
{
	INIT_TIMER;
	START_TIMER;

	std::thread Launch(ExecuteLauncher);
	Launch.detach();

	while (!Application::GetInstancePtr()->GetEstablishState())
	{

	}

	//Initialize Game
	Application::GetInstancePtr()->Initialize(argv[0], iVec2{ 800, 600 }, "Game");
	STOP_TIMER("Initialization took: ");
	//Update Game
	Application::GetInstancePtr()->Update();
	//Cleanup Game
	Application::GetInstancePtr()->Cleanup();

	//End
	return 0;
}