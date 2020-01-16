
#include <ws2tcpip.h>
#include <string>
#include <winsock.h>
#include <thread>
#include <map>

#include "ra_network.h"
#include "ra_application.h"
#include "input\ra_inputhandler.h"

#define DEFAULT_PORT 12307

bool run = false;

void Network::InitializeClient()	
{
	WSADATA wsaData;
	int fResult = 0;
	fResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Error handling.
	if (fResult != 0)
	{
		printf("WSAStartup failed: %d\n", fResult);
		return;
	}

	// Setup our socket address structure

	//Create ServerSocket for clients to connect to.
	this->opponentSocket = INVALID_SOCKET;

	//Create Socket object. 
	this->opponentSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//Error handling.
	if (this->opponentSocket == INVALID_SOCKET)
	{
		printf("Failed to execute socket(). Error Code: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	run = true;

	std::string	oppIp = Application::GetInstancePtr()->GetOpponent();

	sockaddr_in service;
	service.sin_family = AF_INET; //Ipv4 Address
	inet_pton(AF_INET, oppIp.c_str(), &service.sin_addr.s_addr);
	service.sin_port = htons(DEFAULT_PORT); //Set port.

	this->opponentAddr = service;
	int iResult;

	run = true;

	char buffer[sizeof(DataPacket)];

	int addrSize = sizeof(this->opponentAddr);

	while (run)
	{
		iResult = sendto(this->opponentSocket, (const char*)&Input::GetInstancePtr()->GetMyData(), sizeof(DataPacket), 0, (sockaddr*)&this->opponentAddr, sizeof(this->opponentAddr));
	
		iResult = recvfrom(this->opponentSocket, buffer, sizeof(buffer), 0, (sockaddr*)&this->opponentAddr, &addrSize);

		run = Application::GetInstancePtr()->GetRunState();

		if (iResult > 1)
		{
			DataPacket packet = DataPacket();
			packet = *(DataPacket*)&buffer;

			Input::GetInstancePtr()->GetOpponentData() = packet;
			printf("Left %d , Right %d, Shoot %d from client \n", packet.left, packet.right, packet.shoot);
		}
	}

	this->Cleanup();
}

void Network::InitializeHost()
{
	WSADATA wsaData;
	int fResult = 0;

	//Initialize WinSock
	fResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Error handling.
	if (fResult != 0)
	{
		printf("WSAStartup failed: %d\n", fResult);
		WSACleanup();
		return;
	}

	//Create ListenSocket for clients to connect to.
	this->opponentSocket = INVALID_SOCKET;

	//Create Socket object. 
	this->opponentSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//Error handling.
	if (this->opponentSocket == INVALID_SOCKET)
	{
		printf("Failed to execute socket(). Error Code: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	sockaddr_in service;
	service.sin_family = AF_INET; //Ipv4 Address
	service.sin_addr.s_addr = INADDR_ANY; //Any IP interface of server is connectable (LAN, public IP, etc).
	service.sin_port = htons(DEFAULT_PORT); //Set port.

	std::string oppIp = Application::GetInstancePtr()->GetOpponent();

	sockaddr_in addr;
	addr.sin_family = AF_INET; //Ipv4 Address
	inet_pton(AF_INET, oppIp.c_str(), &addr.sin_addr.s_addr);
	addr.sin_port = htons(DEFAULT_PORT); //Set port.

	this->opponentAddr = addr;

	// Bind UDP socket.
	fResult = bind(this->opponentSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service));

	//Error handling.
	if (fResult == SOCKET_ERROR)
	{
		printf("Failed to execute bind(). Error Code: %d\n", WSAGetLastError());
		closesocket(this->opponentSocket);
		WSACleanup();
		return;
	}

	run = true;

	char buffer[sizeof(DataPacket)];

	int addrSize = sizeof(this->opponentAddr);
	int iResult;

	while (run)
	{
		iResult = recvfrom(this->opponentSocket, buffer, sizeof(buffer), 0, (sockaddr*)&this->opponentAddr, &addrSize);

		run = Application::GetInstancePtr()->GetRunState();
		
		if (iResult > 1)
		{
			DataPacket packet = DataPacket();
			packet = *(DataPacket*)&buffer;

			Input::GetInstancePtr()->GetOpponentData() = packet;
			printf("Left %d , Right %d, Shoot %d from client \n", packet.left, packet.right, packet.shoot);
		}

		iResult = sendto(this->opponentSocket, (const char*)&Input::GetInstancePtr()->GetMyData(), sizeof(DataPacket), 0, (sockaddr*)&this->opponentAddr, sizeof(this->opponentAddr));
	}

	this->Cleanup();
}

void Network::Cleanup()
{
	//Shutdown send connection to Server.
	int rResult = shutdown(this->opponentSocket, SD_SEND);

	//Error handling.
	if (rResult == SOCKET_ERROR)
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(this->opponentSocket);
		WSACleanup();
		return;
	}

	//Close connection to Server, cleanup WinSock and delete the window.
	closesocket(opponentSocket);
	WSACleanup();
}
