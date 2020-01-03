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

//Messages as the text on the window.
std::vector<HWND> messages;

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
		SetFocus(hwnd); //Set focus to the window.
		break;
	case WM_KEYDOWN: //If a key is pressed:
	{
		switch (wParam)
		{
		case 38: //Up button.
		{
			if (messages.size() != 0)
			{
				//Get the position of the lowest message.
				RECT Rect;
				GetWindowRect(messages[messages.size() - 1], &Rect);
				MapWindowPoints(HWND_DESKTOP, hwndChat, (LPPOINT)&Rect, 2);

				//If its not over the screen.
				if (true)
				{
					//Move all messages up by 3 units.
					for (HWND text : messages)
					{
						GetWindowRect(text, &Rect);
						MapWindowPoints(HWND_DESKTOP, hwndChat, (LPPOINT)&Rect, 2);
						SetWindowPos(text, 0, Rect.left, Rect.top - 3, Rect.right, Rect.bottom, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
					}
				}
			}
			break;
		}
		case 40: //Down button.
		{
			if (messages.size() != 0)
			{
				//Get the position of the uppest message.
				RECT Rect;
				GetWindowRect(messages[0], &Rect);
				MapWindowPoints(HWND_DESKTOP, hwndChat, (LPPOINT)&Rect, 2);

				printf("Rect top: %d", Rect.top);

				//If its over the screen.
				if (Rect.top < 1)
				{
					//Move all messages down by 3 units.
					for (HWND text : messages)
					{
						GetWindowRect(text, &Rect);
						MapWindowPoints(HWND_DESKTOP, hwndChat, (LPPOINT)&Rect, 2);
						SetWindowPos(text, 0, Rect.left, Rect.top + 3, Rect.right, Rect.bottom, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
					}
				}
			}
			break;
		}
		}
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
		case CreateRoom_ID:
		{
			std::string mess(g_windowMapping2[hwnd]->GetName());
			mess += "'s Room";
			g_windowMapping2[hwnd]->SetRecievedMessage(mess);
			g_windowMapping2[hwnd]->GetRecievedState() = true;

			Data data = Data();

			data.ID = 1;

			g_windowMapping2[hwnd]->SetData(data);

			g_windowMapping2[hwnd]->GetSendState() = true;
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

void LWindow::SetLoginData(LoginData loginData)
{
	this->loginData = loginData;
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
		HWND hwndCreateButton = HWND(); 
		hwndCreateButton = CreateWindowEx(0, "BUTTON", "Create Room",
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

		//Create message text 22 units under the lowest message.
		hwndChatText = CreateWindowEx(0, "static", message.c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, Rect.top + 22, 500, 22, hwndChat,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(hwndChat, -6), NULL);

		//If spawned message is under the screen.
		if (Rect.top + 22 > 380)
		{
			//Move all messages up by top - 380 units.
			for (HWND text : messages)
			{
				RECT textRect;
				GetWindowRect(text, &textRect);
				MapWindowPoints(HWND_DESKTOP, hwndChat, (LPPOINT)&textRect, 2);
				SetWindowPos(text, 0, textRect.left, textRect.top - 22, textRect.right, textRect.bottom, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
			}
		}
	}

	//Save message to global message vector.s
	messages.push_back(hwndChatText);

}

Display* LWindow::GetDisplay(ui32 displayID)
{
	return &g_displays2[displayID];
}

Data LWindow::GetData()
{
	return this->data;
}

void LWindow::SetData(Data data)
{
	this->data = data;
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