//EXTERNAL INCLUDES
#include <unordered_map>
//INTERNAL INCLUDES
#include "ra_lwindow.h"

#define LoginButton_ID 0 //ID for send button events.
#define Name_ID 1 //ID for send button events.
#define Password_ID 2 //ID for send button events.
#define CreateRoom_ID 3 //ID for send button events.

//Global variables needed outside of the window class.
std::vector<Display> g_displays2;
std::unordered_map<HWND, LWindow*> g_windowMapping2;

std::unordered_map<RECT*, HWND> g_roomMapping;

//Messages as the text on the window.
std::vector<HWND> messages;
std::vector<RECT> roomRects;

HWND hwndName; //Name textbox window.
HWND hwndPassword; //Password textbox window.
HWND hwndChat; //Window where the chat is spawned in.
HWND hwndButton;

DECLARE_SINGLETON(LWindow);

//Handles messages for a window.
LRESULT CALLBACK WndProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Handle the messages in the window.
	switch (msg)
	{
	case WM_LBUTTONDOWN: //If Left mouse button is pressed
	{
		SetFocus(hwnd); //Set focus to the window.
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) //Convert command
		{
		case LoginButton_ID: //If its the send button being pressed.
		{
			//Trigger event to send a message.
			//Copy message from the textBox
			LoginData data;

			memset(data.name, 0, 32);
			memset(data.password, 0, 32);

			GetWindowText(hwndName, data.name, 32);

			GetWindowText(hwndPassword, data.password, 32);

			SetWindowText(hwndName, "");
			SetWindowText(hwndPassword, "");
			g_windowMapping2[hwnd]->SetData(data);
			g_windowMapping2[hwnd]->GetQueryState() = true;
			break;
		}
		case CreateRoom_ID:
		{
			std::string mess(g_windowMapping2[hwnd]->GetName());
			mess += "'s Room 1/2";
			g_windowMapping2[hwnd]->SetRecievedMessage(mess);
			g_windowMapping2[hwnd]->GetRecievedState() = true;

			RoomData data;

			memset(data.name, 0, 32);

			data.created = true;

			SET_STRING(data.name, g_windowMapping2[hwnd]->GetName())

			g_windowMapping2[hwnd]->GetCreatedState() = true;
			g_windowMapping2[hwnd]->SetData(data);

			g_windowMapping2[hwnd]->GetSendState() = true;

			DestroyWindow(hwndButton);
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
	hwndButton = HWND();
	hwndButton = CreateWindowEx(0, "BUTTON", "Login",
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

void LWindow::SetData(LoginData loginData)
{
	this->loginData = loginData;
}

void LWindow::SetData(RoomData roomData)
{
	this->roomData = roomData;
}

void LWindow::Update()
{

	if (this->roomActive)
	{
		this->roomActive = false;
		DestroyWindow(hwndName);
		DestroyWindow(hwndPassword);
		DestroyWindow(hwndButton);

		//Create the SendButton
		hwndButton = HWND();
		hwndButton = CreateWindowEx(0, "BUTTON", "Create Room",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, this->handle,
			(HMENU)CreateRoom_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

		//Create the chat window.
		hwndChat = HWND();
		hwndChat = CreateWindowEx(0, "static", "",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 50, 50, 340, 400, this->handle,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);
	}

	//Spawn new messages when a message is recieved.
	if (LWindow::GetInstancePtr()->GetRecievedState())
	{
		LWindow::GetInstancePtr()->GetRecievedState() = false;
		LWindow::GetInstancePtr()->RecievedMessage(LWindow::GetInstancePtr()->GetRecievedMessage());
		LWindow::GetInstancePtr()->SetRecievedMessage("");
		UpdateWindow(LWindow::GetInstancePtr()->GetHandle());
	}

	MSG msg = { };
	if (PeekMessageA(&msg, LWindow::GetInstancePtr()->GetHandle(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

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

bool& LWindow::GetRoomState()
{
	return this->roomActive;
}

LoginData LWindow::GetLoginData()
{
	return this->loginData;
}

RoomData LWindow::GetRoomData()
{
	return this->roomData;
}

void LWindow::RecievedMessage(std::string message)
{
	HWND hwndChatText;
	RECT Rect;

	if (messages.size() == 0)
	{
		//Create message text at top position if its the first one.
		hwndChatText = CreateWindowEx(0, "static", message.c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, 1, 500, 22, hwndChat,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(hwndChat, -6), NULL);
	}
	else
	{
		//Get position of the lowest message.
		GetWindowRect(messages[messages.size() - 1], &Rect);
		MapWindowPoints(HWND_DESKTOP, hwndChat, (LPPOINT)&Rect, 2);

		//If spawned message is under the screen.
		if (Rect.top + 22 > 380)
		{
			printf("Maximum");
			return;
		}

		//Create message text 22 units under the lowest message.
		hwndChatText = CreateWindowEx(0, "static", message.c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, Rect.top + 22, 500, 22, hwndChat,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(hwndChat, -6), NULL);
	}

	g_roomMapping[&Rect] = hwndChatText;

	//Save message to global message vector.s
	messages.push_back(hwndChatText);
	roomRects.push_back(Rect);
}

Display* LWindow::GetDisplay(ui32 displayID)
{
	return &g_displays2[displayID];
}

bool& LWindow::GetRecievedState()
{
	return this->recieve;
}

std::string LWindow::GetRecievedMessage()
{
	return this->currentMessage;
}

void LWindow::SetRecievedMessage(std::string message)
{
	this->currentMessage = message;
}

std::string LWindow::GetName()
{
	return this->name;
}

void LWindow::SetName(std::string name)
{
	this->name = name;
}

bool& LWindow::GetSendState()
{
	return this->send;
}

bool& LWindow::GetCreatedState()
{
	return this->created;
}