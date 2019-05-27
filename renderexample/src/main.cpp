#include "ra_window.h"
#include "ra_rendering.h"

int main(int argc, const char* argv[])
{
	Window* window = new Window;
	window->Instantiate(1366, 786, 0, "windowTest");

	Rendering* rendering = new Rendering;
	rendering->Initialize("RA");

	while (window && window->GetState() != Window::WindowState::Closed)
	{
		if (!window->PollEvents())
		{
			// game loop
		}
	}

	delete window;
	delete rendering;

	return 0;
}