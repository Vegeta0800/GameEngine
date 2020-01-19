
#pragma once
//EXTERNAL INCLUDES
#include <winsock.h>
//INTERNAL INCLUDES

class Input;

//Network class that receives and sends opponents/clients data.
class Network
{
public:
	//Initialize Client, Receive and Send data from/to opponent and Cleanup
	void InitializeClient();
	//Initialize Host, Receive and Send data from/to opponent and Cleanup
	void InitializeHost();
	//Cleanup Winsock
	void Cleanup();

private:
	sockaddr_in opponentAddr;
	SOCKET opponentSocket;

	bool host;
	bool run;
};