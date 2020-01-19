
#pragma once
//EXTERNAL INCLUDES
#include <Windows.h>
#include <string>
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_types.h"
#include "ra_display.h"

//Launcher window class
class LWindow
{
	//Singleton
	DEFINE_SINGLETON(LWindow)
public:
	
	//States the window can be in
	enum class LWindowState
	{
		Started,
		Closed
	};

	//Instantiating the window with specific values
	void Instantiate(ui32 width, ui32 height, ui32 displayID, const char* title);
	//Update launcher window each tick
	void Update();
	//Create new visible window with input data //TODO IMPROVE MAYBE
	void RecievedData(std::string data);
	//Update room to either 1/2 from 2/2 or 2/2 from 1/2
	void UpdateRoom(std::string name, bool joined);
	//Delete a room
	void DeleteRoom(std::string name);


	//Add a display
	void AddDisplay(Display& display);
	//Set the launcher window state
	void SetState(LWindowState state);
	//Set the launcher windows login data
	void SetData(LoginData loginData);
	//Set the launcher windows room data
	void SetData(RoomData roomData);
	//Set received data
	void SetRecievedData(std::string data);
	//Set clients name
	void SetName(std::string name);
	//Set next name
	void SetNextName(std::string name);


	//Get launcher main window handle
	HWND GetHandle();
	//Get display of a display ID
	Display* GetDisplay(ui32 displayID);
	//Get launcher windows state
	LWindowState GetState(void);

	//Get Rogin Data
	LoginData GetLoginData();
	//Get Room Data
	RoomData GetRoomData();

	//Get received data
	std::string GetRecievedData();
	//Get name
	std::string GetName();
	//Get next name
	std::string GetNextName();
	//Get reference to the deleted room name
	std::string& GetDeleteRoom();

	//Get login request state
	bool& GetQueryState();
	//Get state if login was succesful and room frame window should be created
	bool& GetRoomState();
	//Get ready state of client
	bool& GetReadyState();
	//Get if client has created a room.
	bool& GetCreatedState();
	//Get if data is received to show.
	bool& GetRecievedState();
	//Get if ready message should be send.
	bool& GetSendReadyMessage();
	//Get if data should be send
	bool& GetSendState();
	
private:
	std::string nextName; //For locating last room created. 
	std::string name;

	std::string currentData;
	std::string deleteRoom = "";
	
	HWND handle = 0;
	
	LoginData loginData;
	RoomData roomData;

	LWindowState state = LWindowState::Started;

	ui32 height = 0;
	ui32 width = 0;

	//states
	bool send = false;
	bool recieve = false;
	bool created = false;
	bool queryLogin = false;
	bool roomActive = false;
	bool ready = false;
	bool update = false; //TODO
	bool sendReady = false;
};