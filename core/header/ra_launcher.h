
#pragma once
//EXTERNAL INCLUDES
#include <winsock.h>
#include <thread>
//INTERNAL INCLUDES

//Launcher class where you can login and start the game from
class Launcher
{
public:
	//Startup launcher
	bool Startup();
	//Send data
	void Send();
	//Cleanup
	void Cleanup();

private:
	bool startGame = false;
	SOCKET ServerSocket;

	std::thread recieveData;
	std::thread windowHandling;
};