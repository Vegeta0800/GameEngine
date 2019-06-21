
//EXTERNAL INCLUDES
#include <unordered_map>
//INTERNAL INCLUDES
#include "ra_window.h"


std::vector<Display> g_displays;
std::unordered_map<HWND, Window*> g_windowMapping;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Handle the messages in the window.
	switch (msg)
	{
	case WM_SIZE:

		//If the message is destroy, destroy the window.
	case WM_DESTROY:
		DestroyWindow(hwnd);
		break;
		//If the message is close, stop proccessing messages.
	case WM_CLOSE:
		g_windowMapping[hwnd]->SetState(Window::WindowState::Closed);
		PostQuitMessage(0);
		break;
	default:
		//Go on.
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
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
		g_displays.push_back(currentMonitor);
	}

	return TRUE;
}

DECLARE_SINGLETON(Window)

void Window::Instantiate(ui32 width, ui32 height, ui32 displayID, const char* title)
{
	this->filesystem = new Filesystem();

	//Initialize variable
	ui32 style = 0;
	this->width = width;
	this->height = height;

	//Enumerate through the monitors and return if return if it wasnt succesful
	if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0))
		return;

	//Create a struct to hold window data and fill that struct with data.
	WNDCLASSEXA wndex = { 0 };
	wndex.cbSize = sizeof(WNDCLASSEX);
	wndex.lpfnWndProc = WndProc;
	wndex.hInstance = GetModuleHandle(NULL);
	wndex.hIcon = NULL;
	wndex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndex.lpszClassName = title;

	//Register a window and if that fails crash the program.
	if (!RegisterClassExA(&wndex))
		throw;

	//Set the window style.
	style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_THICKFRAME;

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

	//If that fails crash the program.
	if (this->handle == NULL)
		throw;

	g_windowMapping[this->handle] = this;

	this->state = Window::WindowState::Started;

	//Show and update the window.
	ShowWindow(this->handle, SW_SHOW);
	UpdateWindow(this->handle);
}

bool Window::PollEvents(void)
{
	MSG msg;
	if (PeekMessageA(&msg, this->handle, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
		return true;
	}
	return false;
}

void Window::AddDisplay(Display& display)
{
	g_displays.push_back(display);
}

void Window::SetState(WindowState state)
{
	this->state = state;
}

Window::WindowState Window::GetState()
{
	return this->state;
}

Display* Window::GetDisplay(ui32 displayID)
{
	return &g_displays[displayID];
}

ui32 Window::GetWidth(void)
{
	return this->width;
}

ui32 Window::GetHeight(void)
{
	return this->height;
}

HWND& Window::GetHandle(void)
{
	return this->handle;
}

Filesystem* Window::GetFileSystem(void)
{
	return this->filesystem;
}