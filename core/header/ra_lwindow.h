#pragma once
//EXTERNAL INCLUDES
#include <Windows.h>
#include <vector>
#include <string>
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_types.h"
#include "ra_display.h"

#pragma pack(push, 1)
struct LoginData
{
	std::string name;
	std::string password;
};
#pragma pack(pop)

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
	void SetLoginData(LoginData loginData);

	//Get Display by ID
	Display* GetDisplay(ui32 displayID);

	//Get current window state
	LWindowState GetState(void);

	//Get handle to main window
	HWND GetHandle();

	//Get states for messages

	bool& GetQueryState();

	//Get LoginData for session
	LoginData GetLoginData();

private:
	HWND handle = 0;
	LWindowState state = LWindowState::Started;

	ui32 width = 0;
	ui32 height = 0;

	LoginData loginData;

	bool send = false;
	bool recieve = false;

	bool queryLogin = false;
};