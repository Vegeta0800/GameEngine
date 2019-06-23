
#include "ra_application.h"
#include "ra_utils.h"
#include "console/ra_cvar.h"
#include "console/ra_console.h"
#include "ra_window.h"
#include "ra_rendering.h"
#include "input/ra_inputhandler.h"

DECLARE_SINGLETON(Application)

void Application::Initialize(const char* path, iVec2 resolution, const char* title)
{
	std::string gamepathstr = static_cast<std::string>(path);
	for (char& sign : gamepathstr)
	{
		if (sign == '\\')
		{
			sign = '/';
		}
	}
	gamepathstr = gamepathstr.substr(0, gamepathstr.find_last_of("/") + 1);
	CVar gamepath = { "gamepath", gamepathstr.c_str() };
	Console::GetInstancePtr()->SetCVar(gamepath);

	this->filesystem = new Filesystem;
	this->renderer = new Rendering;

	Window::GetInstancePtr()->Instantiate(800, 600, 0, "windowTest");


	this->renderer->Initialize("RenderingExample", VK_MAKE_VERSION(0, 0, 0));
}

void Application::Update()
{
	//INIT_TIMER  

	auto gameStartTime = std::chrono::high_resolution_clock::now();

	while (Window::GetInstancePtr() && Window::GetInstancePtr()->GetState() != Window::WindowState::Closed)
	{
		//START_TIMER

		auto frameTime = std::chrono::high_resolution_clock::now();

		float timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(gameStartTime - frameTime).count() / 1000.0f;

		if (!Window::GetInstancePtr()->PollEvents())
		{
			//root->Update();
		}

		this->renderer->Update(timeDifference);

		if (Input::GetInstancePtr()->GetUpState())
			Input::GetInstancePtr()->EradicateUpKeys();

		//STOP_TIMER("Loop took")
	}

}

void Application::Cleanup()
{
	this->renderer->Cleanup();
	this->filesystem->Cleanup();
	Window::GetInstancePtr()->Release();
	Console::GetInstancePtr()->CleanUp();
	Console::GetInstancePtr()->Release();

	delete this->renderer;
}

Filesystem* Application::GetFilesystem()
{
	return this->filesystem;
}

Rendering* Application::GetRenderer()
{
	return this->renderer;
}