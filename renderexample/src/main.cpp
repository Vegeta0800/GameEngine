
//EXTERNAL INCLUDES
#include <thread>
//INTERNAL INCLUDES
#include "ra_application.h"
#include "ra_launcher.h"
#include "ra_utils.h"
#include "ra_types.h"

//Start game
bool stopGame = false;

//Launcher thread function
void ExecuteLauncher()
{
	Launcher launcher = Launcher();

	stopGame = launcher.Startup();
}

void Loop(const char* argv[])
{
	while (!Application::GetInstancePtr()->GetEstablishState())
	{

	}

	//Initialize Game
	Application::GetInstancePtr()->Initialize(argv[0], iVec2{ 800, 600 }, "Game");
	//Update Game
	Application::GetInstancePtr()->Update();
	//Cleanup Game
	Application::GetInstancePtr()->Cleanup();
}

int main(int argc, const char* argv[])
{
	std::thread Launch(ExecuteLauncher);
	Launch.detach();
	
	Loop(argv);

	//End
	return 0;
}