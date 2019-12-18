//EXTERNAL INCLUDES
#include <unordered_map>
//INTERNAL INCLUDES
#include "ra_lwindow.h"

#define LoginButton_ID 0 //ID for send button events.
#define Name_ID 1 //ID for send button events.
#define Password_ID 2 //ID for send button events.

#define StartMatch_ID 3


//Global variables needed outside of the window class.
std::vector<Display> g_displays2;
std::unordered_map<HWND, LWindow*> g_windowMapping2;

HWND hwndName; //Name textbox window.
HWND hwndPassword; //Password textbox window.

DECLARE_SINGLETON(LWindow);

//Handles messages for a window.
LRESULT CALLBACK WndProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Handle the messages in the window.
	switch (msg)
	{
	case WM_LBUTTONDOWN: //If Left mouse button is pressed
		SetFocus(hwnd); //Set focus to the window.
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) //Convert command
		{
		case LoginButton_ID: //If its the send button being pressed.
		{
			//Trigger event to send a message.
			//Copy message from the textBox
			char* name = new char;
			GetWindowText(hwndName, name, 300);

			char* password = new char;
			GetWindowText(hwndPassword, password, 300);

			LoginData data = { std::string(name), std::string(password) };

			SetWindowText(hwndName, "");
			SetWindowText(hwndPassword, "");
			g_windowMapping2[hwnd]->SetLoginData(data);
			g_windowMapping2[hwnd]->GetQueryState() = true;
			break;
		}
		}
		break;
	}
	//If the message is destroy, destroy the window.
	case WM_DESTROY:
	{
		g_windowMapping2[hwnd]->SetState(LWindow::LWindowState::Closed); //Set closed state
		DestroyWindow(hwnd); //Destroy the window.
		break;
	}
	//If the message is close, stop proccessing messages.
	case WM_CLOSE:
	{
		g_windowMapping2[hwnd]->SetState(LWindow::LWindowState::Closed); //Set closed state
		PostQuitMessage(0);
		break;
	}
	default:
		//Go on.
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

//Save display information.
BOOL CALLBACK MonitorEnumProc2(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	//Initialize variables.
	MONITORINFO info;
	info.cbSize = sizeof(info);

	//Get the monitor info of the inputed monitor.
	if (GetMonitorInfo(hMonitor, &info))
	{
		//Save the Monitor information into the struct display.
		Display currentMonitor;

		currentMonitor.width = abs(info.rcMonitor.left - info.rcMonitor.right);
		currentMonitor.height = abs(info.rcMonitor.top - info.rcMonitor.bottom);

		currentMonitor.posX = info.rcMonitor.left;
		currentMonitor.posY = info.rcMonitor.top;

		//Add the display into windows displays list.
		g_displays2.push_back(currentMonitor);
	}

	return TRUE;
}


void LWindow::Instantiate(ui32 width, ui32 height, ui32 displayID, const char* title)
{
	//Initialize variable
	ui32 style = 0;
	this->width = width;
	this->height = height;

	//Enumerate through all monitors connected to the computer and return if there are none.
	if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc2, 0))
		return;

	//Create a struct to hold window data and fill that struct with data.
	WNDCLASSEXA wndex = { 0 };
	wndex.cbSize = sizeof(WNDCLASSEX);
	wndex.lpfnWndProc = WndProc2; //Set this function as the function to handle the messages.
	wndex.hInstance = GetModuleHandle(NULL);
	wndex.hIcon = NULL;
	wndex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndex.hbrBackground = CreateSolidBrush(RGB(255, 210, 0)); //Yellow
	wndex.lpszClassName = title;

	//Register a window and if that fails crash the program.
	if (!RegisterClassExA(&wndex))
		throw;

	//Set the window style.
	style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	//Get the selected display.
	Display* selectedDisplay = GetDisplay(displayID);

	//Get the center position of the display.
	i64 posX = ((selectedDisplay->width - width) / 2) + selectedDisplay->posX;
	i64 posY = ((selectedDisplay->height - height) / 2) + selectedDisplay->posY;

	//Fill the RECT struct with data.
	RECT adjustedRect = RECT
	{
		static_cast<LONG>(0),
		static_cast<LONG>(0),
		static_cast<LONG>(width),
		static_cast<LONG>(height)
	};

	//Adjust the Window. If that fails crash the program.
	if (!AdjustWindowRect(&adjustedRect, style, FALSE))
		throw;

	//Create a handle onto a newly created window.
	this->handle = CreateWindowExA
	(
		NULL,
		title,
		title,
		style,
		static_cast<int>(posX),
		static_cast<int>(posY),
		adjustedRect.right,
		adjustedRect.bottom,
		NULL,
		NULL,
		GetModuleHandle(NULL), NULL
	);

	//Create the LoginButton
	HWND hwndButton = HWND();
	hwndButton = CreateWindowEx(0, "BUTTON", "Send Message",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, this->handle,
		(HMENU)LoginButton_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

	//Create the name box.
	hwndName = HWND();
	hwndName = CreateWindowEx(0, "EDIT", "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN | ES_MULTILINE,
		10, 420, 300, 25, this->handle, (HMENU)Name_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), 0);

	//Create the password box.
	hwndPassword = HWND(); 
	hwndPassword = CreateWindowEx(0, "EDIT", "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN | ES_MULTILINE,
		10, 480, 300, 25, this->handle, (HMENU)Password_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), 0);

	//If that fails crash the program.
	if (this->handle == NULL)
		throw;

	//Set the window to the main window handle.
	g_windowMapping2[this->handle] = this;

	//Set window state
	this->state = LWindow::LWindowState::Started;

	//Show window and draw it.
	ShowWindow(this->handle, SW_SHOW);
	UpdateWindow(this->handle);
}

void LWindow::AddDisplay(Display& display)
{
	g_displays2.push_back(display);
}

void LWindow::SetState(LWindowState state)
{
	this->state = state;
}

void LWindow::SetLoginData(LoginData loginData)
{
	this->loginData = loginData;
}

LWindow::LWindowState LWindow::GetState()
{
	return this->state;
}

HWND LWindow::GetHandle()
{
	return this->handle;
}

bool& LWindow::GetQueryState()
{
	return this->queryLogin;
}

LoginData LWindow::GetLoginData()
{
	return this->loginData;
}

Display* LWindow::GetDisplay(ui32 displayID)
{
	return &g_displays2[displayID];
}