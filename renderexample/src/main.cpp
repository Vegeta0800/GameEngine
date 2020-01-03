
#include "ra_application.h"
#include "ra_utils.h"
#include "ra_launcher.h"
#include "ra_types.h"

int main(int argc, const char* argv[])
{
	LoginData lD = {"hey", "we"};

	Data data;
	data.ID = 0;
	data.data = (char*)&lD;

	const char* buffer = (const char*)&data;

	char buff[12];

	for (int i = 0; i < 12; i++)
	{
		buff[i] = buffer[i];
	}

	Data data2 = *(Data*)buff;

	LoginData lD2;
	lD2 = *(LoginData*)data2.data;





	INIT_TIMER;
	START_TIMER;

	Launcher launcher = Launcher();

	launcher.Startup();

	Application::GetInstancePtr()->Initialize(argv[0], iVec2{ 800, 600 }, "Game");
	STOP_TIMER("Initialization took: ");
	
	Application::GetInstancePtr()->Update();
	Application::GetInstancePtr()->Cleanup();

	return 0;
}