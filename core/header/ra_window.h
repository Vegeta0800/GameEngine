
#pragma once
//EXTERNAL INCLUDES
#include <Windows.h>
#include <vector>
//INTERNAL INCLUDES
#include "ra_types.h"
#include "ra_display.h"


class Window
{
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
	int GetWidth(void);
	int GetHeigth(void);

private:
	HWND handle;	
	WindowState state;

	int width;
	int height;
};