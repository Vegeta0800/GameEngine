
#include <ws2tcpip.h>
#include <string>
#include <winsock.h>
#include <thread>

#include "ra_network.h"
#include "ra_application.h"
#include "input\ra_inputhandler.h"

bool run = false;

void Recieve(SOCKET opponent, sockaddr_in addr)
{
	char buffer[256];

	int addrSize = sizeof(addr);

	while (run)
	{
		int iResult = recvfrom(opponent, buffer, sizeof(buffer), 0, (sockaddr*)&addr, &addrSize);

		if (iResult > 1)
		{
			Data data = Data();
			data = *(Data*)&buffer;

			printf("Received %d bytes from opponent \n", iResult);

			Input::GetInstancePtr()->GetOpponentData() = data;
		}
		else if (iResult == 0)
		{
			run = false;
		}
		else
		{
			run = false;
		}
	}

	int iResult = shutdown(opponent, SD_SEND);

	//Error handling.
	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to execute shutdown(). Error Code: %d\n", WSAGetLastError());
		closesocket(opponent);
		return;
	}

	closesocket(opponent);
}

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

	// Connect to server.
	fResult = connect(this->opponentSocket, reinterpret_cast<SOCKADDR*>(&Application::GetInstancePtr()->GetOpponent()), sizeof(SOCKADDR_IN));

	//Error handling.
	if (fResult == SOCKET_ERROR)
	{
		printf("Unable to connect to server! Error Code: %ld\n", WSAGetLastError());
		closesocket(this->opponentSocket);
		this->opponentSocket = INVALID_SOCKET;
	}

	if (this->opponentSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!");
		WSACleanup();
		return;
	}

	run = true;

	//Create thread for handling data thats recieved by the server. Pass the server socket as argument.
	this->recieveData = std::thread(std::bind(Recieve, this->opponentSocket, this->opponentAddr));

	this->Send();
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

	// Bind UDP socket.
	fResult = bind(this->opponentSocket, reinterpret_cast<SOCKADDR*>(&Application::GetInstancePtr()->GetOpponent()), sizeof(SOCKADDR_IN));

	//Error handling.
	if (fResult == SOCKET_ERROR)
	{
		printf("Failed to execute bind(). Error Code: %d\n", WSAGetLastError());
		closesocket(this->opponentSocket);
		WSACleanup();
		return;
	}

	run = true;

	this->recieveData = std::thread(std::bind(Recieve, this->opponentSocket, this->opponentAddr));

	this->Send();
}

void Network::Send()
{
	while (run)
	{
		int iResult = sendto(this->opponentSocket, (const char*)&Input::GetInstancePtr()->GetMyData(), 256, 0, (sockaddr*)&this->opponentAddr, sizeof(this->opponentAddr));
		
		printf("send %d bytes to server\n", iResult);

		//Error handling.
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(this->opponentSocket);
			WSACleanup();
			return;
		}
	}

	this->Cleanup();

	this->recieveData.join();
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

void Network::SetAddr(sockaddr_in addr)
{
	this->opponentAddr = addr;
}
