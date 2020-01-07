#pragma once
#include <winsock.h>

class Launcher
{
public:
	bool Startup();
	void Login();

	void Exit();

private:
	SOCKET ServerSocket;

	bool startConnection;

	SOCKADDR_IN enemyAddress;
};