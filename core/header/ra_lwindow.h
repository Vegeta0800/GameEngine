#pragma once
//EXTERNAL INCLUDES
#include <Windows.h>
#include <vector>
#include <string>
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_types.h"
#include "ra_display.h"

class LWindow
{
	DEFINE_SINGLETON(LWindow)
public:
	enum class LWindowState
	{
		Started,
		Closed
	};

	//Create window
	void Instantiate(ui32 width, ui32 height, ui32 displayID, const char* title);

	//Add a display
	void AddDisplay(Display& display);

	//Set window state
	void SetState(LWindowState state);

	//Set messages
	void SetData(LoginData loginData);
	void SetData(RoomData roomData);

	void Update();

	//Get Display by ID
	Display* GetDisplay(ui32 displayID);

	//Get current window state
	LWindowState GetState(void);

	void RecievedMessage(std::string message);
	std::string GetRecievedMessage();

	std::string GetName();
	void SetName(std::string name);

	std::string GetNextName();
	void SetNextName(std::string name);

	void SetRecievedMessage(std::string message);

	void UpdateRoom(std::string name);
	void DeleteRoom(std::string name);

	//Get handle to main window
	HWND GetHandle();

	//Get states for messages
	bool& GetQueryState();
	bool& GetRoomState();
	bool& GetSendState();

	bool& GetCreatedState();

	bool& GetRecievedState();

	bool& GetReadyState();
	bool& GetSendReadyMessage();

	std::string& GetDeleteRoom();

	//Get LoginData for session
	LoginData GetLoginData();
	RoomData GetRoomData();

private:
	HWND handle = 0;
	LWindowState state = LWindowState::Started;

	ui32 width = 0;
	ui32 height = 0;

	LoginData loginData;
	RoomData roomData;

	std::string name;
	std::string nextName;

	std::string currentMessage;

	std::string deleteRoom = "";

	bool send = false;
	bool recieve = false;

	bool created = false;

	bool queryLogin = false;

	bool roomActive = false;

	bool ready = false;
	bool update = false;
	bool sendReady = false;
};