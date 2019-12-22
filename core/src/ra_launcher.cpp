
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "ra_lwindow.h"
#include "ra_launcher.h"
#include "ra_application.h"
#include <winsock.h>


// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT 12307 //Use any port you want. Has to be in port forwarding settings.
#define SERVER_IP "127.0.0.1" //Public IP used for clients outside of servers network. (public IPv4 of server)
//#define SERVER_IP "" //LAN Address used for clients inside of servers network, because they cant connect to the public IP due to the NAT. (LAN IPv4 of Server)

//Global variables used by all threads.
bool running = true;
bool loggedIn = false;
bool sendData = false;

DWORD WINAPI WindowHandling(LPVOID lpParameter)
{
	//Create the window.
	LWindow::GetInstancePtr()->Instantiate(450, 550, 0, "GameLauncher");

	//While the window is opened
	while (LWindow::GetInstancePtr()->GetState() == LWindow::LWindowState::Started && running)
	{
		//Check for incoming window messages and handle them.
		LWindow::GetInstancePtr()->Update();
	}

	running = false;

	//End application.
	return 0;
}

//Handles recieving data from the server with the server socket as a parameter.
DWORD WINAPI RecieveData(LPVOID lpParameter)
{
	//Parameter conversion
	SOCKET ServerSocket = (SOCKET)lpParameter;

	char buff[1];
	int rResult;

	//Receive data until the server closes the connection
	do
	{
		//Recieve data
		rResult = recv(ServerSocket, buff, 1, 0);

		//If there is data recieved
		if (rResult > 0)
		{
			if (!loggedIn)
			{
				if (buff[0] == 1)
				{
					//Trigger event so the window spawns a new text with the message.
					printf("Recieved %d bytes from server \n", rResult);
					printf("Logged in");
					loggedIn = true;

					LWindow::GetInstancePtr()->GetRoomState() = true;
				}
			}
			else
			{
				if (buff[0] == 1)
				{
					sendData = true;
				}
			}
		}
		//If connection is closing:
		else if (rResult == 0)
		{
			printf("Connection closed\n");
			running = false;
		}
		//If failed:
		else
		{
			printf("recv failed: %d\n", WSAGetLastError());
			running = false;
		}

	} while (running);

	return 0;
}

void Launcher::Startup()
{
	running = true;

	DWORD dwThreadIdWindow;
	CreateThread(NULL, 0, WindowHandling, NULL, 0, &dwThreadIdWindow);

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
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(DEFAULT_PORT); //Set port
	SockAddr.sin_family = AF_INET; //IPv4 Address
	inet_pton(AF_INET, SERVER_IP, &SockAddr.sin_addr.s_addr); //Convert the server IP to the sockaddr_in format and set it.

	//Create ServerSocket for clients to connect to.
	this->ServerSocket = INVALID_SOCKET;

	//Create Socket object. 
	this->ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Error handling.
	if (this->ServerSocket == INVALID_SOCKET)
	{
		printf("Failed to execute socket(). Error Code: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// Connect to server.
	fResult = connect(this->ServerSocket, reinterpret_cast<SOCKADDR*>(&SockAddr), sizeof(SOCKADDR_IN));

	//Error handling.
	if (fResult == SOCKET_ERROR)
	{
		printf("Unable to connect to server! Error Code: %ld\n", WSAGetLastError());
		closesocket(this->ServerSocket);
		this->ServerSocket = INVALID_SOCKET;
	}

	if (this->ServerSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!");
		WSACleanup();
		return;
	}

	//Create thread for handling data thats recieved by the server. Pass the server socket as argument.
	DWORD dwThreadId;
	CreateThread(NULL, 0, RecieveData, (LPVOID)this->ServerSocket, 0, &dwThreadId);

	this->Login();
}

void Launcher::Login()
{
	//While the window is opened.
	while (running)
	{
		if (loggedIn)
		{

		}
		else
		{
			//When the send button is pressed and a message is in the textbox.
			if (LWindow::GetInstancePtr()->GetQueryState())
		{
			//Send that message to the server.
			LoginData data = LWindow::GetInstancePtr()->GetLoginData();

			int iResult = send(this->ServerSocket, (char*)&data, sizeof(LoginData), 0);

			int s = sizeof(LoginData);
			int size = sizeof(std::string);

			printf("send %d bytes to server (%d)", iResult, size);

			//Error handling.
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(this->ServerSocket);
				WSACleanup();
				return;
			}

			LWindow::GetInstancePtr()->GetQueryState() = false;
		}
		}
	}

	this->Exit();
}

void Launcher::Exit()
{
	//Shutdown send connection to Server.
	int rResult = shutdown(ServerSocket, SD_SEND);

	//Error handling.
	if (rResult == SOCKET_ERROR)
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}

	//Close connection to Server, cleanup WinSock and delete the window.
	closesocket(ServerSocket);
	WSACleanup();
	LWindow::GetInstancePtr()->Release();

	return;
}