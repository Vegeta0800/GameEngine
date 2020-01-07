
#include "ra_application.h"
#include "ra_utils.h"
#include "ra_launcher.h"
#include "ra_types.h"

int main(int argc, const char* argv[])
{
	INIT_TIMER;
	START_TIMER;

	Launcher launcher = Launcher();

	if (launcher.Startup())
	{
		Application::GetInstancePtr()->Initialize(argv[0], iVec2{ 800, 600 }, "Game");
		STOP_TIMER("Initialization took: ");

		Application::GetInstancePtr()->Update();
		Application::GetInstancePtr()->Cleanup();
	}

	return 0;
}