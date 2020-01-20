
//EXTERNAL INCLUDES
#include <ws2tcpip.h>
#include <string>
#include <winsock.h>
#include <thread>
#include <map>
//INTERNAL INCLUDES
#include "ra_network.h"
#include "input\ra_inputhandler.h"
#include "ra_application.h"

#define DEFAULT_PORT 12307 //Port of server

//Initialize Client, Receive and Send data from/to opponent and Cleanup
void Network::InitializeClient()	
{
	//Startup winsock
	WSADATA wsaData;
	int fResult = 0;
	fResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Error handling.
	if (fResult != 0)
	{
		printf("WSAStartup failed: %d\n", fResult);
		return;
	}

	//Create opponentSocket for client to send/receive data to/from.
	this->opponentSocket = INVALID_SOCKET;

	//Create Socket object. 
	this->opponentSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP

	//Error handling.
	if (this->opponentSocket == INVALID_SOCKET)
	{
		printf("Failed to execute socket(). Error Code: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	this->run = true;

	//Get opponents IP
	std::string	oppIp = Application::GetInstancePtr()->GetOpponent();

	//Create sockadd_in structure with opponents IP address
	this->opponentAddr = sockaddr_in();
	this->opponentAddr.sin_family = AF_INET; //IPv4 Address
	inet_pton(AF_INET, oppIp.c_str(), &this->opponentAddr.sin_addr.s_addr); //Convert IP address to format
	this->opponentAddr.sin_port = htons(DEFAULT_PORT); //Set port.

	int iResult; //Result for functions

	//Create buffer and size
	char buffer[sizeof(DataPacket)];
	int addrSize = sizeof(this->opponentAddr);

	//Hole punching NAT
	iResult = sendto(this->opponentSocket, "H", 1, 0, (sockaddr*)&this->opponentAddr, sizeof(this->opponentAddr));

	//While running
	while (this->run)
	{
		this->run = Application::GetInstancePtr()->GetRunState();

		//Send clients data to opponent
		iResult = sendto(this->opponentSocket, (const char*)&Input::GetInstancePtr()->GetMyData(), sizeof(DataPacket), 0, (sockaddr*)&this->opponentAddr, sizeof(this->opponentAddr));
	
		//Receive opponents data from opponent
		iResult = recvfrom(this->opponentSocket, buffer, sizeof(buffer), 0, (sockaddr*)&this->opponentAddr, &addrSize);

		//Set run state
		this->run = Application::GetInstancePtr()->GetRunState();

		//If something is received and it isnt a hole punching message
		if (iResult > 2)
		{
			//Convert buffer to DataPacket
			DataPacket packet = DataPacket();
			packet = *(DataPacket*)&buffer;

			//Set opponents data to that DataPacket
			Input::GetInstancePtr()->GetOpponentData() = packet;
		}
	}

	printf("Close connection");

	//Cleanup when game concluded.
	this->Cleanup();
}

//Initialize Host, Receive and Send data from/to opponent and Cleanup
void Network::InitializeHost()
{
	//Startup Winsock
	WSADATA wsaData;
	int fResult = 0;

	fResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Error handling.
	if (fResult != 0)
	{
		printf("WSAStartup failed: %d\n", fResult);
		WSACleanup();
		return;
	}

	//Create opponentSocket for host to send/receive data to/from.
	this->opponentSocket = INVALID_SOCKET;

	//Create Socket object. 
	this->opponentSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP

	//Error handling.
	if (this->opponentSocket == INVALID_SOCKET)
	{
		printf("Failed to execute socket(). Error Code: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	//Create hosts socket address information
	sockaddr_in service;
	service.sin_family = AF_INET; //Ipv4 Address
	service.sin_addr.s_addr = INADDR_ANY; //Any IP interface of server is connectable (LAN, public IP, etc).
	service.sin_port = htons(DEFAULT_PORT); //Set port.

	//Get opponents IP
	std::string oppIp = Application::GetInstancePtr()->GetOpponent();

	//Create sockadd_in structure with opponents IP address
	this->opponentAddr = sockaddr_in();
	this->opponentAddr.sin_family = AF_INET; //Ipv4 Address
	inet_pton(AF_INET, oppIp.c_str(), &this->opponentAddr.sin_addr.s_addr); //Convert IP address to format
	this->opponentAddr.sin_port = htons(DEFAULT_PORT); //Set port.

	//Bind hosts UDP socket.
	fResult = bind(this->opponentSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service));

	//Error handling.
	if (fResult == SOCKET_ERROR)
	{
		printf("Failed to execute bind(). Error Code: %d\n", WSAGetLastError());
		closesocket(this->opponentSocket);
		WSACleanup();
		return;
	}

	//Start running
	this->run = true;

	//Create buffer and size
	char buffer[sizeof(DataPacket)];
	int addrSize = sizeof(this->opponentAddr);

	int iResult; //Result for functions

	//Hole punching NAT
	iResult = sendto(this->opponentSocket, "H", 1, 0, (sockaddr*)&this->opponentAddr, sizeof(this->opponentAddr));

	while (this->run)
	{
		//Receive opponents data from opponent
		iResult = recvfrom(this->opponentSocket, buffer, sizeof(buffer), 0, (sockaddr*)&this->opponentAddr, &addrSize);

		//Set run state
		this->run = Application::GetInstancePtr()->GetRunState();
		
		//If something is received and it isnt a hole punching message
		if (iResult > 2)
		{
			//Convert buffer to DataPacket
			DataPacket packet = DataPacket();
			packet = *(DataPacket*)&buffer;

			//Set opponents data to that DataPacket
			Input::GetInstancePtr()->GetOpponentData() = packet;
		}

		//Send hosts data to opponent
		iResult = sendto(this->opponentSocket, (const char*)&Input::GetInstancePtr()->GetMyData(), sizeof(DataPacket), 0, (sockaddr*)&this->opponentAddr, sizeof(this->opponentAddr));
	}

	//Cleanup when game concluded.
	this->Cleanup();
}

//Cleanup Winsock
void Network::Cleanup()
{
	//Shutdown send connection.
	int rResult = shutdown(this->opponentSocket, SD_SEND);

	//Error handling.
	if (rResult == SOCKET_ERROR)
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(this->opponentSocket);
		WSACleanup();
		return;
	}

	//Close connection, cleanup WinSock and delete the window.
	closesocket(opponentSocket);
	WSACleanup();
}
