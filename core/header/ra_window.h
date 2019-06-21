
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

	enum class WindowState
	{
		Started,
		Closed
	};

	void Instantiate(ui32 width, ui32 height, ui32 displayID, const char* title);
	bool PollEvents(void);
	void AddDisplay(Display& display);
	void SetState(WindowState state);
	
	Display* GetDisplay(ui32 displayID);
	WindowState GetState(void);
	ui32 GetWidth(void);
	ui32 GetHeight(void);
	HWND& GetHandle(void);
private:
	HWND handle = 0;	
	WindowState state = WindowState::Started;
	
	ui32 width = 0;
	ui32 height = 0;
};