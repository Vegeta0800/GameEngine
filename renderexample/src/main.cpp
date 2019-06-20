
#include "ra_window.h"
#include "ra_rendering.h"
#include "ra_gameobject.h"
#include "ra_transform.h"
#include "ra_component.h"
#include "ra_utils.h"
#include "console/ra_cvar.h"
#include "console/ra_console.h"

int main(int argc, const char* argv[])
{

	std::string gamepathstr = static_cast<std::string>(argv[0]);
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

	Window::GetInstancePtr()->Instantiate(800, 600, 0, "windowTest");

	Rendering* rendering = new Rendering;
	rendering->Initialize("RenderingExample", VK_MAKE_VERSION(0, 0, 0));

	Gameobject* root = new Gameobject();
	root->MakeRoot();
	
	Gameobject* child = new Gameobject();
	child->SetParent(root);

	Component* component = new Component();
	child->AddComponent(component);
	child->SetName("child1");

	Gameobject* child2 = new Gameobject();
	child2->SetParent(child);
	child2->SetName("child2");

	Gameobject* child3 = new Gameobject();
	child3->SetParent(child2);
	child3->SetName("child3");

	child->GetTransform().position = Math::Vec3::zero;
	child2->GetTransform().position = Math::Vec3{ 10.0f, 0, 3.0f};
	child3->GetTransform().position = Math::Vec3{ -10.0f, 0, 3.0f };

	child->GetTransform().eulerRotation = Math::Vec3::zero;
	child2->GetTransform().eulerRotation = Math::Vec3{ 45.0f, 0, 30.0f };
	child3->GetTransform().eulerRotation = Math::Vec3{ -45.0f, 0, 30.0f };

	INIT_TIMER;


	while (Window::GetInstancePtr() && Window::GetInstancePtr()->GetState() != Window::WindowState::Closed)
	{
		//START_TIMER;
		if (!Window::GetInstancePtr()->PollEvents())
		{
			//root->Update();
		}

		rendering->Update();
		//STOP_TIMER("Loop took: ");
	}

	rendering->Cleanup();

	delete rendering;
	delete Window::GetInstancePtr();

	return 0;
}