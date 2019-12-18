#pragma once
#include <winsock.h>

class Launcher
{
public:
	void Startup();
	void Login();

	void Exit();

private:
	SOCKET ServerSocket;
};