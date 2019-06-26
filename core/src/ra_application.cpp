
#include "ra_application.h"
#include "ra_utils.h"
#include "console/ra_cvar.h"
#include "console/ra_console.h"
#include "ra_window.h"
#include "ra_rendering.h"
#include "input/ra_inputhandler.h"
#include "ra_scenemanager.h"

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
	Window::GetInstancePtr()->Instantiate(800, 600, 0, "windowTest");

	Rendering::GetInstancePtr()->Initialize("RenderingExample", VK_MAKE_VERSION(0, 0, 0));
}

void Application::Update()
{
	INIT_TIMER  

	auto gameStartTime = std::chrono::high_resolution_clock::now();

	while (Window::GetInstancePtr() && Window::GetInstancePtr()->GetState() != Window::WindowState::Closed)
	{
		START_TIMER

		auto frameTime = std::chrono::high_resolution_clock::now();

		float timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(gameStartTime - frameTime).count() / 1000.0f;

		if (!Window::GetInstancePtr()->PollEvents())
		{
			//root->Update();
		}

		Rendering::GetInstancePtr()->Update(timeDifference);

		if (Input::GetInstancePtr()->GetUpState())
			Input::GetInstancePtr()->EradicateUpKeys();

		STOP_TIMER("Loop took")
	}

}

void Application::Cleanup()
{
	SceneManager::GetInstancePtr()->Cleanup();
	Rendering::GetInstancePtr()->Cleanup();
	this->filesystem->Cleanup();
	Window::GetInstancePtr()->Release();
	Console::GetInstancePtr()->CleanUp();
	Console::GetInstancePtr()->Release();
	Rendering::GetInstancePtr()->Release();
}

Filesystem* Application::GetFilesystem()
{
	return this->filesystem;
}