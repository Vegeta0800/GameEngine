
#include "ra_application.h"
#include "ra_utils.h"

int main(int argc, const char* argv[])
{
	INIT_TIMER;
	START_TIMER;
	Application::GetInstancePtr()->Initialize(argv[0], iVec2{ 800, 600 }, "render");
	STOP_TIMER("Initialization took: ");
	
	Application::GetInstancePtr()->Update();
	Application::GetInstancePtr()->Cleanup();

	return 0;
}