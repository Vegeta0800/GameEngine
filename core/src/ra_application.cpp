
#include "ra_application.h"
#include "ra_utils.h"
#include "console/ra_cvar.h"
#include "console/ra_console.h"
#include "ra_window.h"
#include "ra_rendering.h"
#include "input/ra_inputhandler.h"
#include "ra_scenemanager.h"
#include "ra_gameobject.h"
#include <ctime>
#include <chrono>

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
	SceneManager::GetInstancePtr()->Initialize();

	Rendering::GetInstancePtr()->Initialize("RenderingExample", VK_MAKE_VERSION(0, 0, 0));
}

void Application::Update()
{
	INIT_TIMER

	this->deltaTime = 0.0f;

	auto gameStartTime = std::chrono::high_resolution_clock::now();
	auto gameCurrentTime = std::chrono::high_resolution_clock::now();

	while (Window::GetInstancePtr() && Window::GetInstancePtr()->GetState() != Window::WindowState::Closed)
	{
		START_TIMER

		gameStartTime = std::chrono::high_resolution_clock::now();

		this->deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(gameStartTime - gameCurrentTime).count();
		gameCurrentTime = gameStartTime;

		if (!Window::GetInstancePtr()->PollEvents())
		{
			SceneManager::GetInstancePtr()->Update();
			Rendering::GetInstancePtr()->Update();

			if (Input::GetInstancePtr()->GetUpState())
				Input::GetInstancePtr()->EradicateUpKeys();

		}
			//STOP_TIMER("Loop took")
	}

}

void Application::Cleanup()
{
	Rendering::GetInstancePtr()->Cleanup();
	SceneManager::GetInstancePtr()->Cleanup();
	this->filesystem->Cleanup();
	Window::GetInstancePtr()->Release();
	Console::GetInstancePtr()->CleanUp();
	Console::GetInstancePtr()->Release();
	Rendering::GetInstancePtr()->Release();
	SceneManager::GetInstancePtr()->Release();
}

Filesystem* Application::GetFilesystem()
{
	return this->filesystem;
}

float Application::GetDeltaTime()
{
	return this->deltaTime;
}

float& Application::GetAspectRatio()
{
	return this->aspectRatio;
}