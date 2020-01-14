#pragma once
#include <winsock.h>
#include <thread>

class Launcher
{
public:
	bool Startup();
	void Login();

	void Exit();

private:
	bool startGame = false;
	SOCKET ServerSocket;

	std::thread recieveData;
	std::thread windowHandling;
};