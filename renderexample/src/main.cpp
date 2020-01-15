
#include "ra_application.h"
#include "ra_utils.h"
#include "ra_launcher.h"
#include "ra_types.h"
#include <thread>

bool startGame = false;

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

	Application::GetInstancePtr()->Initialize(argv[0], iVec2{ 800, 600 }, "Game");
	STOP_TIMER("Initialization took: ");

	Application::GetInstancePtr()->Update();

	Application::GetInstancePtr()->Cleanup();

	return 0;
}