
#include "ra_window.h"
#include "ra_rendering.h"
#include "ra_gameobject.h"
#include "ra_transform.h"
#include "ra_component.h"
#include "ra_utils.h"

int main(int argc, const char* argv[])
{
	Window::GetInstancePtr()->Instantiate(1366, 786, 0, "windowTest");

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

	while (Window::GetInstancePtr() && Window::GetInstancePtr()->GetState() != Window::WindowState::Closed)
	{
		if (!Window::GetInstancePtr()->PollEvents())
		{
			//root->Update();
		}
	}

	rendering->Cleanup();

	delete rendering;
	delete Window::GetInstancePtr();

	return 0;
}