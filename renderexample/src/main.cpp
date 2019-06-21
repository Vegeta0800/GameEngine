
#include "ra_application.h"

int main(int argc, const char* argv[])
{
	Application::GetInstancePtr()->Initialize(argv[0], iVec2{ 800, 600 }, "render");
	Application::GetInstancePtr()->Update();
	Application::GetInstancePtr()->Cleanup();

	return 0;
}