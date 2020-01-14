
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <string>
#include <winsock.h>
#include <thread>

#include "ra_lwindow.h"
#include "ra_launcher.h"
#include "ra_application.h"


// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT 12307 //Use any port you want. Has to be in port forwarding settings.
#define SERVER_IP "192.168.1.30" //Public IP used for clients outside of servers network. (public IPv4 of server)
//#define SERVER_IP "" //LAN Address used for clients inside of servers network, because they cant connect to the public IP due to the NAT. (LAN IPv4 of Server)

//Global variables used by all threads.
bool running = true;
bool loggedIn = false;
bool inRoom = false;
bool sendData = false;

bool start = false;

void WindowHandling()
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
	return;
}

//Handles recieving data from the server with the server socket as a parameter.
void RecieveData(SOCKET ServerSocket)
{
	//Parameter conversion
	char buff[sizeof(LoginData)];
	int rResult;

	//Receive data until the server closes the connection
	do
	{
		//Recieve data
		if(!loggedIn)
			rResult = recv(ServerSocket, buff, sizeof(LoginData), 0);
		else
			rResult = recv(ServerSocket, buff, sizeof(RoomData), 0);

		printf("Recieved %d bytes from server \n", rResult);

		//If there is data recieved
		if (rResult > 0)
		{
			if (!loggedIn)
			{
				if (buff[0] == 1)
				{
					//Trigger event so the window spawns a new text with the message.
					printf("Logged in\n");
					loggedIn = true;

					LWindow::GetInstancePtr()->SetName(LWindow::GetInstancePtr()->GetLoginData().name);
					LWindow::GetInstancePtr()->GetRoomState() = true;
				}
				else
				{
					printf("Login failed!\n");
				}
			}
			else
			{
				if (rResult <= 16)
				{
					std::string ip = buff;
					ip.resize(rResult);

					sockaddr_in opp;
					opp.sin_family = AF_INET; //Ipv4 Address
					if(ip[rResult - 1] == 1)
						opp.sin_addr.s_addr = INADDR_ANY; //Any IP interface of server is connectable (LAN, public IP, etc).
					else if(ip[rResult - 1] == 0)
						opp.sin_addr.s_addr = inet_pton(AF_INET, ip.c_str(), &opp.sin_addr.s_addr); //Any IP interface of server is connectable (LAN, public IP, etc).
					opp.sin_port = htons(DEFAULT_PORT); //Set port.

					Application::GetInstancePtr()->GetEstablishState() = true;
					Application::GetInstancePtr()->SetOpponent(opp);
				} 
				else
				{
					RoomData data = RoomData();
					data = *(RoomData*)&buff;

					std::string name = data.name;

					if (data.deleted == true)
					{
						LWindow::GetInstancePtr()->GetDeleteRoom() = name;

						if (!data.created)
						{
							std::string mess(name);
							mess += "'s Room 1/2";
							LWindow::GetInstancePtr()->SetNextName(name);
							LWindow::GetInstancePtr()->SetRecievedMessage(mess);
							LWindow::GetInstancePtr()->GetRecievedState() = true;
						}
					}
					else
					{
						if (data.created == true)
						{
							if (data.name != LWindow::GetInstancePtr()->GetName())
							{
								std::string mess(name);
								mess += "'s Room 1/2";
								LWindow::GetInstancePtr()->SetNextName(name);
								LWindow::GetInstancePtr()->SetRecievedMessage(mess);
								LWindow::GetInstancePtr()->GetRecievedState() = true;
							}
						}
						else
						{
							LWindow::GetInstancePtr()->UpdateRoom(name);
							printf("Updating room: %s \n", name.c_str());
						}
					}
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

	return;
}

bool Launcher::Startup()
{
	running = true;

	this->windowHandling = std::thread(WindowHandling);

	WSADATA wsaData;
	int fResult = 0;
	fResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Error handling.
	if (fResult != 0)
	{
		printf("WSAStartup failed: %d\n", fResult);
		return false;
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
		return false;
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
		return false;
	}

	//Create thread for handling data thats recieved by the server. Pass the server socket as argument.
	this->recieveData = std::thread(std::bind(RecieveData, this->ServerSocket));

	this->Login();

	return this->startGame;
}

void Launcher::Login()
{
	//While the window is opened.
	while (running)
	{
		if (loggedIn)
		{
			if (LWindow::GetInstancePtr()->GetSendReadyMessage())
			{
				LWindow::GetInstancePtr()->GetSendReadyMessage() = false;

				char b = LWindow::GetInstancePtr()->GetReadyState();
				int sResult = send(ServerSocket, (const char*)&b, 1, 0);

				if (sResult == SOCKET_ERROR)
				{
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(ServerSocket);
					WSACleanup();
					return;
				}
			}

			if (LWindow::GetInstancePtr()->GetSendState())
			{
				LWindow::GetInstancePtr()->GetSendState() = false;

				RoomData data = LWindow::GetInstancePtr()->GetRoomData();

				//Send that message to the server.
				int sResult = send(ServerSocket, (const char*)&data, sizeof(data), 0);
				printf("Sending %d bytes... \n", sResult);

				//Error handling.
				if (sResult == SOCKET_ERROR)
				{
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(ServerSocket);
					WSACleanup();
					return;
				}

				LWindow::GetInstancePtr()->SetData(RoomData());
			}
		}
		else
		{
			//When the send button is pressed and a message is in the textbox.
			if (LWindow::GetInstancePtr()->GetQueryState())
			{
				//Send that message to the server.
				LoginData loginData = LWindow::GetInstancePtr()->GetLoginData();

				int iResult = send(this->ServerSocket, (const char*)&loginData, sizeof(loginData), 0);

				printf("send %d bytes to server\n", iResult);

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

	this->windowHandling.join();
	this->recieveData.join();
}

void Launcher::Exit()
{
	//Shutdown send connection to Server.
	int rResult = shutdown(this->ServerSocket, SD_SEND);

	//Error handling.
	if (rResult == SOCKET_ERROR)
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(this->ServerSocket);
		WSACleanup();
		return;
	}

	//Close connection to Server, cleanup WinSock and delete the window.
	closesocket(ServerSocket);
	WSACleanup();
	LWindow::GetInstancePtr()->Release();

	return;
}