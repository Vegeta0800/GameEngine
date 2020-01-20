
#pragma once
//EXTERNAL INCLUDES
#include <Windows.h>
#include <vector>
//INTERNAL INCLUDES
#include "ra_types.h"
#include "ra_display.h"
#include "ra_utils.h"
#include "filesystem/ra_filesystem.h"

class Window
{
	DEFINE_SINGLETON(Window)

public:
	//States the main window can be in
	enum class WindowState
	{
		Pending,
		Started,
		Closed
	};

	//Instantiating the window with specific values
	void Instantiate(ui32 width, ui32 height, ui32 displayID, const char* title);
	//Show active window and update it
	void ShowActiveWindow();
	//Destroy window
	void Destroy(void);
	//Check if there are any events and if so handle them and return true
	//If not return false
	bool PollEvents(void);


	//Add a display
	void AddDisplay(Display& display);

	//Set windows state
	void SetState(WindowState state);
	

	//Get Handle of main window
	HWND& GetHandle(void);

	//Get the windows state
	WindowState GetState(void);

	//Get a display by ID
	Display* GetDisplay(ui32 displayID);

	//Get Width of main window
	ui32 GetWidth(void);
	//Get Height of main window
	ui32 GetHeight(void);

private:
	WindowState state = WindowState::Started;
	HWND handle = 0;	
	
	ui32 width = 0;
	ui32 height = 0;
};