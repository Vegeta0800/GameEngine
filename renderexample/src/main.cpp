
#include "ra_application.h"
#include "ra_utils.h"
#include "ra_launcher.h"
#include "ra_types.h"
#include <thread>

void ExecuteLauncher()
{
	Launcher launcher = Launcher();

	if (!launcher.Startup()) return;
}

int main(int argc, const char* argv[])
{
	INIT_TIMER;
	START_TIMER;

	//std::thread Launch(ExecuteLauncher);

	//while (!Application::GetInstancePtr()->GetEstablishState())
	//{

	//}

	Application::GetInstancePtr()->Initialize(argv[0], iVec2{ 800, 600 }, "Game");
	STOP_TIMER("Initialization took: ");

	Application::GetInstancePtr()->Cleanup();

	return 0;
}