
//EXTERNAL INCLUDES
#include <ws2tcpip.h>
#include <string>
#include <winsock.h>
#include <thread>
#include <sstream>
#include <algorithm>
//INTERNAL INCLUDES
#include "ra_launcher.h"
#include "ra_lwindow.h"
#include "ra_application.h"


// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT 12307 //Use any port you want. Has to be in port forwarding settings.
//#define SERVER_IP "192.168.1.30" //Public IP used for clients outside of servers network. (public IPv4 of server)
#define SERVER_IP "127.0.0.1" //LAN Address used for clients inside of servers network, because they cant connect to the public IP due to the NAT. (LAN IPv4 of Server)

//Global variables used by all threads.
bool running = true;
bool loggedIn = false;
bool sendData = false;

bool inGame = false;

//Hashing function to send password
std::string Hash(std::string input)
{
	//magic number and hash
	unsigned int magic = 397633456;
	unsigned int hash = 729042348;

	//XOR all characters with the hash number
	//Multiply that times the magic number
	for (int i = 0; i < input.length(); i++)
	{
		hash = hash ^ (input[i]);
		hash = hash * magic;
	}

	std::string hexHash;
	std::stringstream hexStream;

	//Convert that number to hex number with fixed size
	hexStream << std::hex << hash;
	hexHash = hexStream.str();
	std::transform(hexHash.begin(), hexHash.end(), hexHash.begin(), ::toupper);

	return hexHash; //Return the hex hash
}

//Window thread
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

	//End launcher.
	running = false;

	return;
}
//Handles recieving data from the server with the server socket as a parameter.
void RecieveData(SOCKET ServerSocket)
{
	//Create buffer
	char buffer[sizeof(LoginData)];
	//Result for functions
	int rResult;

	memset(buffer, 0, sizeof(LoginData));

	//Receive data until the server closes the connection
	do
	{
		//While the client isnt in game
		if (!inGame)
		{
			//Recieve data
			if (!loggedIn)
				rResult = recv(ServerSocket, buffer, sizeof(LoginData), 0); //Receive login data if not logged in
			else
				rResult = recv(ServerSocket, buffer, sizeof(RoomData), 0); //Receive other data like RoomData, IP, or end data if logged in

			printf("Recieved %d bytes from server \n", rResult);

			//If there is data recieved
			if (rResult > 0)
			{
				//If not logged in yet
				if (!loggedIn)
				{
					//Check if login was succesful or not.
					if (buffer[0] == 1)
					{
						//Trigger event so the window spawns a new window with rooms.
						loggedIn = true;
						printf("Logged in\n");

						//Set the name of the client
						LWindow::GetInstancePtr()->SetName(LWindow::GetInstancePtr()->GetLoginData().name);
						//And trigger the event
						LWindow::GetInstancePtr()->GetRoomState() = true;
					}
					else
					{
						printf("Login failed!\n"); // Wrong Login Data
					}
				}
				//If already logged in
				else
				{
					//If the server sent 16 bytes, that means its the IP address of opponent and the game can start
					if (rResult <= 16)
					{
						Application::GetInstancePtr()->GetHostState() = buffer[15]; //Check last byte to check if client is host or client

						//Get IP address out of buffer
						std::string ip = buffer;
						ip.resize(rResult - 1);

						//Begin application init
						Application::GetInstancePtr()->GetEstablishState() = true;
						Application::GetInstancePtr()->SetOpponent(ip); //Set opponents IP
						inGame = true;
					}
					//Otherwise its Room data
					else
					{
						//Cast buffer to RoomData
						RoomData data = RoomData();
						data = *(RoomData*)&buffer;

						//Get room data's name as a string
						std::string name = data.name;

						//If its the delete message
						if (data.deleted == true)
						{
							//If still not the host or still client inside.
							if (!data.created)
							{
								//Update room
								LWindow::GetInstancePtr()->UpdateRoom(name, false);
							}
							else
							{
								//Delete the visible room 
								LWindow::GetInstancePtr()->GetDeleteRoom() = name;
							}
						}
						//If it isnt a delete message
						else
						{
							//If it is a create message
							if (data.created == true)
							{
								//If its a create message from someone else
								if (data.name != LWindow::GetInstancePtr()->GetName())
								{
									//Create new room message with data's name
									std::string data_s(name);
									data_s += "'s Room 1/2";
									LWindow::GetInstancePtr()->SetNextName(name);

									//Signal to launcher window to spawn new visible room
									LWindow::GetInstancePtr()->SetRecievedData(data_s);
									LWindow::GetInstancePtr()->GetRecievedState() = true;
								}
							}
							//If its a join message
							else
							{
								//Update the room to 2 people.
								LWindow::GetInstancePtr()->UpdateRoom(name, true);
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
				//Stop launcher
				running = false;
			}
			//If failed:
			else
			{
				printf("recv failed: %d\n", WSAGetLastError());
				//Stop launcher
				running = false;
			}
		}

	} while (running);

	return;
}

//Startup launcher
bool Launcher::Startup()
{
	//Startup WinSocket
	WSADATA wsaData;
	int fResult = 0;
	fResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Error handling.
	if (fResult != 0)
	{
		printf("WSAStartup failed: %d\n", fResult);
		return false;
	}

	//Setup socket address structure
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(DEFAULT_PORT); //Set port
	SockAddr.sin_family = AF_INET; //IPv4 Address
	inet_pton(AF_INET, SERVER_IP, &SockAddr.sin_addr.s_addr); //Convert the server IP to the sockaddr_in format and set it.

	//Create ServerSocket for client to connect to.
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

	//Connect to server.
	fResult = connect(this->ServerSocket, reinterpret_cast<SOCKADDR*>(&SockAddr), sizeof(SOCKADDR_IN));

	//Error handling.
	if (fResult == SOCKET_ERROR)
	{
		printf("Unable to connect to server! Error Code: %ld\n", WSAGetLastError());
		closesocket(this->ServerSocket);
		this->ServerSocket = INVALID_SOCKET;
		return false;
	}
	//Error handling.
	if (this->ServerSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!");
		WSACleanup();
		return false;
	}

	//Start sending and receiving
	running = true;

	//Create thread for launcher window
	this->windowHandling = std::thread(WindowHandling);
	//Create thread for handling data thats recieved by the server. Pass the server socket as argument.
	this->recieveData = std::thread(std::bind(RecieveData, this->ServerSocket));

	//Start sending data
	this->Send();

	//Once finished return if launcher was succesful or if something failed.
	return this->startGame;
}
//Send data
void Launcher::Send()
{
	//While the window is opened.
	while (running)
	{
		//If already logged in
		if (loggedIn && !inGame)
		{
			//If in a room and ready button is pressed
			if (LWindow::GetInstancePtr()->GetSendReadyMessage())
			{
				//Reset bool triggered by button
				LWindow::GetInstancePtr()->GetSendReadyMessage() = false;

				//Send if client is ready or not to server
				char b = LWindow::GetInstancePtr()->GetReadyState();
				int sResult = send(ServerSocket, (const char*)&b, 1, 0);

				//Error handling.
				if (sResult == SOCKET_ERROR)
				{
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(ServerSocket);
					WSACleanup();
					return;
				}

				continue;
			}
			//If not in room yet. TODO EXIT ROOM IN ROOM
			else if (LWindow::GetInstancePtr()->GetSendState())
			{
				//Reset bool triggered by button
				LWindow::GetInstancePtr()->GetSendState() = false;

				//Get Room data from launcher window
				RoomData data = LWindow::GetInstancePtr()->GetRoomData();

				//Send that data to the server.
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

				//Reset Room data in launcher window.
				LWindow::GetInstancePtr()->SetData(RoomData());
			}
		}
		//If not logged in and not in game
		else if(!loggedIn && !inGame)
		{
			//When the window's Login button is pressed
			if (LWindow::GetInstancePtr()->GetQueryState())
			{
				//Get login data from launcher window.
				LoginData loginData = LWindow::GetInstancePtr()->GetLoginData();
				
				std::string password = loginData.password; // Get password to string

				//Hash password
				password = Hash(password);

				//Rewrite loginData's password field
				memset(&loginData.password, 0, 32);
				SET_STRING(loginData.password, password);

				//Send login data to server
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

				//Set send state, triggered by the button click, to false
				LWindow::GetInstancePtr()->GetQueryState() = false;
			}
		}
		//If game concluded
		else if(this->sendResult && inGame)
		{
			//Reset bools
			this->sendResult = false;
			inGame = false;

			//Get winner and store it into char
			char c;
			if (Application::GetInstancePtr()->GetWinnerState())
				c = 1;
			else
				c = 0;

			//Send result to server
			int iResult = send(this->ServerSocket, &c, sizeof(c), 0);

			printf("send %d bytes to server\n", iResult);

			//Error handling.
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(this->ServerSocket);
				WSACleanup();
				return;
			}
		}
	}
	
	//Wait for window thread to conclude.
	this->windowHandling.join();
	//Shutdown server and cleanup everything.
	this->Cleanup();
	//Wait for receive thread to conclude.
	this->recieveData.join();
}
//Cleanup
void Launcher::Cleanup()
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

	//Close connection to Server, cleanup WinSock and release the launcher window's instance.
	closesocket(ServerSocket);
	WSACleanup();
	LWindow::GetInstancePtr()->Release();
	return;
}

//End game TODO
void Launcher::EndGame()
{
	inGame = false;
	this->sendResult = true;
}