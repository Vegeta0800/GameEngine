
//EXTERNAL INCLUDES
#include <unordered_map>
#include <map>
//INTERNAL INCLUDES
#include "ra_lwindow.h"

//Windows ID's TODO
#define LoginButton_ID 0 //ID for send button events.
#define Name_ID 1 //ID for send button events.
#define Password_ID 2 //ID for send button events.
#define CreateRoom_ID 3 //ID for send button events.
#define Ready_ID 4

//Global variables needed outside of the window class.
std::vector<Display> g_launcherDisplays;
std::unordered_map<HWND, LWindow*> g_launcherWindowMapping;

//Room mappings
std::unordered_map<int, std::string> g_roomMapping;
std::unordered_map<std::string, HWND> g_roomStringMapping;

//Messages as the text on the window.
std::vector<HWND> rooms;
std::vector<RECT> roomRects;

HWND hwndName; //Name textbox window.
HWND hwndPassword; //Password textbox window.
HWND hwndRooms; //Window where the rooms are spawned in.
HWND hwndButton; //Window that is the button that is active.

//Singleton
DECLARE_SINGLETON(LWindow);

//Handles messages for launcher window.
LRESULT CALLBACK LaunchWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Handle the messages in the window. TODO ENTER AND TAB
	switch (msg)
	{
	case WM_LBUTTONDBLCLK: //If Left mouse double clicks
	{
		//If there are rooms already
		if (roomRects.size() != 0)
		{
			SetFocus(hwnd); //Set focus to the launcher window.

			POINT pt;
			GetCursorPos(&pt); //Get cursor position

			//For all room RECTS
			for (int i = 0; i < roomRects.size(); i++)
			{
				//Check if the cursor positon is inside that RECT
				if (PtInRect(&roomRects[i], pt))
				{
					//If so set room data with the clients name and configure it as a join message
					RoomData data;
					memset(data.name, 0, 32);
					data.created = false;
					data.deleted = false;

					//Set datas name field
					std::string name = g_roomMapping[i];
					SET_STRING(data.name, name);

					//Set the data to the launcher windows data
					g_launcherWindowMapping[hwnd]->SetData(data);
					g_launcherWindowMapping[hwnd]->GetSendState() = true; //Send the data

					//Destroy the Create Window button
					DestroyWindow(hwndButton);

					//TODO CREATE LEAVE ROOM BUTTON

					//Create Ready button
					hwndButton = HWND();
					hwndButton = CreateWindowEx(0, "BUTTON", "Ready",
						WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, g_launcherWindowMapping[hwnd]->GetHandle(),
						(HMENU)Ready_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(g_launcherWindowMapping[hwnd]->GetHandle(), -6), NULL);
				}
			}
		}

		//If not do nothing.
		break;
	}
	case WM_COMMAND: //If window registers a command
	{
		switch (LOWORD(wParam)) //Convert command
		{
		case LoginButton_ID: //If its the login button being pressed.
		{
			//Copy message from the textBox
			
			//Initialize login data
			LoginData data;

			//Set name and passwords field to \0
			memset(data.name, 0, 32);
			memset(data.password, 0, 32);

			//Get the name and passwords input text windows text and set it to the login datas fields
			GetWindowText(hwndName, data.name, 32);
			GetWindowText(hwndPassword, data.password, 32);

			//Set the name and passwords input text window texts to null.
			SetWindowText(hwndName, "");
			SetWindowText(hwndPassword, "");

			//Set launcher windows data to it.
			g_launcherWindowMapping[hwnd]->SetData(data);
			//Trigger event to send a loginData.
			g_launcherWindowMapping[hwnd]->GetQueryState() = true;
			break;
		}
		case CreateRoom_ID: //If its a create room button being pressed
		{
			//Get name and create string message for a room
			std::string s_data(g_launcherWindowMapping[hwnd]->GetName());
			s_data += "'s Room 1/2";

			//Spawn visible room on clients screen.
			g_launcherWindowMapping[hwnd]->SetNextName(g_launcherWindowMapping[hwnd]->GetName()); //Set next name for creation placement
			g_launcherWindowMapping[hwnd]->SetRecievedData(s_data);
			g_launcherWindowMapping[hwnd]->GetRecievedState() = true;

			//Initialize room data
			RoomData data;

			//Set name field to null
			memset(data.name, 0, 32);

			//Set it to a create message
			data.created = true;
			data.deleted = false;

			//Set name field to clients name.
			SET_STRING(data.name, g_launcherWindowMapping[hwnd]->GetName())
				
			//Created state is true
			g_launcherWindowMapping[hwnd]->GetCreatedState() = true;

			//Set data 
			g_launcherWindowMapping[hwnd]->SetData(data);
			//Send data
			g_launcherWindowMapping[hwnd]->GetSendState() = true;

			//Destroy the create room button
			DestroyWindow(hwndButton);

			//TODO CREATE LEAVE ROOM BUTTON

			//Create Ready button
			hwndButton = HWND();
			hwndButton = CreateWindowEx(0, "BUTTON", "Ready",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, g_launcherWindowMapping[hwnd]->GetHandle(),
				(HMENU)Ready_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(g_launcherWindowMapping[hwnd]->GetHandle(), -6), NULL);
			
			break;
		}
		case Ready_ID: //If its a ready button being pressed
		{
			//Trigger the ready state to swap and send that message.
			g_launcherWindowMapping[hwnd]->GetReadyState() = !g_launcherWindowMapping[hwnd]->GetReadyState();
			g_launcherWindowMapping[hwnd]->GetSendReadyMessage() = true;
			break;
		}
		}
		break;
	}
	//If the message is destroy, destroy the window.
	case WM_DESTROY:
	{
		g_launcherWindowMapping[hwnd]->SetState(LWindow::LWindowState::Closed); //Set closed state
		DestroyWindow(hwnd); //Destroy the window.
		break;
	}
	//If the message is close, stop proccessing messages.
	case WM_CLOSE:
	{
		g_launcherWindowMapping[hwnd]->SetState(LWindow::LWindowState::Closed); //Set closed state
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
		g_launcherDisplays.push_back(currentMonitor);
	}

	return TRUE;
}


//Instantiating the window with specific values
void LWindow::Instantiate(ui32 width, ui32 height, ui32 displayID, const char* title)
{
	//Initialize and set variables
	ui32 style = 0;
	this->width = width;
	this->height = height;

	//Enumerate through all monitors connected to the computer and return if there are none.
	if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc2, 0))
		return;

	//Create a struct to hold window data and fill that struct with data.
	WNDCLASSEXA wndex = { 0 };
	wndex.cbSize = sizeof(WNDCLASSEX);
	wndex.lpfnWndProc = LaunchWndProc; //Set this function as the function to handle the messages on another thread.
	wndex.hInstance = GetModuleHandle(NULL);
	wndex.hIcon = NULL;
	wndex.hCursor = LoadCursor(NULL, IDC_ARROW); //Cursor
	wndex.hbrBackground = CreateSolidBrush(RGB(255, 210, 0)); //Yellow
	wndex.lpszClassName = title;
	wndex.style = CS_DBLCLKS; //Activate double clicks

	//Register a window and if that fails crash the program.
	if (!RegisterClassExA(&wndex))
		throw;

	//Set extra window styles.
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
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN,
		10, 420, 300, 25, this->handle, (HMENU)Name_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), 0);

	//Create the password box.
	hwndPassword = HWND(); 
	hwndPassword = CreateWindowEx(0, "EDIT", "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN | ES_PASSWORD,
		10, 480, 300, 25, this->handle, (HMENU)Password_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), 0);

	//If that fails crash the program.
	if (this->handle == NULL)
		throw;

	//Set the window to the main window handle.
	g_launcherWindowMapping[this->handle] = this;

	//Set window state
	this->state = LWindow::LWindowState::Started;

	//Show window and draw it.
	ShowWindow(this->handle, SW_SHOW);
	UpdateWindow(this->handle);
}

//Update launcher window each tick
void LWindow::Update()
{
	//If login was succesful spawn new window content
	if (this->roomActive)
	{
		//Deactivate spawning
		this->roomActive = false;

		//If reset launcher
		if (rooms.size() != 0)
		{
			//Clear all rooms
			for (int i = 0; i < rooms.size(); i++)
			{
				//Messages as the text on the window.
				DestroyWindow(rooms[i]);
			}
			g_roomMapping.clear();
			g_roomStringMapping.clear();
			roomRects.clear();
			rooms.clear();
		}
		//If first time
		else
		{
			//Destroy name, password and button window.
			DestroyWindow(hwndName);
			DestroyWindow(hwndPassword);
		}

		//Destroy button
		DestroyWindow(hwndButton);

		//Create the Create Room button
		hwndButton = HWND();
		hwndButton = CreateWindowEx(0, "BUTTON", "Create Room",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, this->handle,
			(HMENU)CreateRoom_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

		//Create the room window.
		hwndRooms = HWND();
		hwndRooms = CreateWindowEx(0, "static", "",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 50, 50, 340, 400, this->handle,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);
	}
	//If a room is supposed to be deleted
	else if (this->deleteRoom != "")
	{
		//Stop deletion
		this->deleteRoom = "";

		//Destroy requested room
		DestroyWindow(g_roomStringMapping[this->deleteRoom]);
		g_roomStringMapping.erase(this->deleteRoom);
	}
	//If a room is supposed to be updated TODO redundant if ready is instantly created when created room
	else if (this->update)
	{
		//Stop updating room
		this->update = false;

		//Destroy create room button.
		DestroyWindow(hwndButton);

		//Create ready button
		hwndButton = HWND();
		hwndButton = CreateWindowEx(0, "BUTTON", "Ready",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, this->handle,
			(HMENU)Ready_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);
	}

	//Spawn new windows when data is recieved.
	if (this->GetRecievedState())
	{
		this->GetRecievedState() = false;
		this->RecievedData(this->GetRecievedData());
		this->SetRecievedData("");
	}

	//If window events occur
	MSG msg = { };
	if (PeekMessageA(&msg, this->handle, 0, 0, PM_REMOVE))
	{
		//Translate and handle them
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//Update the main launcher window
	UpdateWindow(this->handle);
}

//Create new visible window with input data //TODO IMPROVE MAYBE
void LWindow::RecievedData(std::string data)
{
	//Create temp window handle and RECT
	HWND hwndRoomsText;
	RECT Rect;

	//If there are no rooms yet
	if (rooms.size() == 0)
	{
		//Create a visible room at top position if its the first one.
		hwndRoomsText = CreateWindowEx(0, "static", data.c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, 1, (5 * (static_cast<int>(data.size()) + 10)), 22, hwndRooms,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(hwndRooms, -6), NULL);
	}
	//Else if there are rooms
	else
	{
		//Get position of the lowest visible room.
		GetWindowRect(rooms[rooms.size() - 1], &Rect);
		MapWindowPoints(HWND_DESKTOP, hwndRooms, (LPPOINT)&Rect, 2); //Map to room frame

		//If spawned visible room is under the screen. TODO SCROLL MAYBE?
		if (Rect.top + 22 > 380)
		{
			printf("Maximum");
			return;
		}

		//Create visible room text 22 units under the visible room message.
		hwndRoomsText = CreateWindowEx(0, "static", data.c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, Rect.top + 22, (5 * (static_cast<int>(data.size()) + 10)), 22, hwndRooms,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(hwndRooms, -6), NULL);
	}

	//Get the newly spawned visible room
	RECT rect;
	GetWindowRect(hwndRoomsText, &rect);

	//Store the rooms index in mapping using the next name
	g_roomMapping[static_cast<int>(roomRects.size())] = this->GetNextName();
	roomRects.push_back(rect); //push newly spawned visible room rect to roomRects 
	g_roomStringMapping[this->GetNextName()] = hwndRoomsText; //Map rooms name to window handle

	//Push room to all rooms.
	rooms.push_back(hwndRoomsText);
}

//Update room to either 1/2 from 2/2 or 2/2 from 1/2
void LWindow::UpdateRoom(std::string name, bool joined)
{
	//If joined
	if (joined)
	{
		//Update room to ... 2/2
		this->update = true;
		std::string data(name);
		data += "'s Room 2/2";

		SetWindowText(g_roomStringMapping[name], data.c_str());
	}
	//If left
	else
	{
		//Update room to ... 1/2
		std::string data(name);
		data += "'s Room 1/2";
		SetWindowText(g_roomStringMapping[name], data.c_str());
	}
}

//Delete a room
void LWindow::DeleteRoom(std::string name)
{
	//If there are no rooms return
	if (rooms.size() == 0) return;

	//Get wanted room rect
	RECT roomRect;
	GetWindowRect(g_roomStringMapping[name], &roomRect);
	MapWindowPoints(HWND_DESKTOP, hwndRooms, (LPPOINT)&roomRect, 2);

	RECT rect;

	//Move all other rooms that are under this room up
	int roomIndex = 0;
	if (rooms.size() != 1)
	{
		for (int i = 0; i < (int)rooms.size(); i++)
		{
			if (rooms[i] == g_roomStringMapping[name])
				roomIndex = i;

			GetWindowRect(rooms[i], &rect);
			MapWindowPoints(HWND_DESKTOP, hwndRooms, (LPPOINT)&rect, 2);

			//Check if wanted room is higher than the other room
			if (rect.top > roomRect.top)
			{
				//If so move up the other room by 22 units.
				SetWindowPos(rooms[i], 0, rect.left, rect.top - 22, rect.right, rect.bottom, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
			}
		}
	}

	//Delete index from room mapping
	g_roomMapping.erase(roomIndex);

	//Swap wanted room with last room and pop back the vector
	HWND temp = rooms[rooms.size() - 1];
	rooms[rooms.size() - 1] = rooms[roomIndex];
	rooms[roomIndex] = temp;
	rooms.pop_back();

	//Get the window rect and destroy the window afterwards
	GetWindowRect(g_roomStringMapping[name], &roomRect);
	DestroyWindow(g_roomStringMapping[name]);
	
	//Search for the window with the same rect info and do the same trick as for the rooms.
	int rectIndex = 0;
	if (roomRects.size() != 1)
	{
		for (int i = 0; i < roomRects.size(); i++)
		{
			if (roomRects[i].top == roomRect.top
				&& roomRects[i].bottom == roomRect.bottom
				&& roomRects[i].left == roomRect.left
				&& roomRects[i].right == roomRect.right)
			{
				RECT tempRect = roomRects[roomRects.size() - 1];
				roomRects[roomRects.size() - 1] = roomRects[rectIndex];
				roomRects[rectIndex] = tempRect;
				roomRects.pop_back();
				break; //stop
			}
		}
	}

	//Erase the rooms name from mapping
	g_roomStringMapping.erase(name);
}


//Add a display
void LWindow::AddDisplay(Display& display)
{
	g_launcherDisplays.push_back(display);
}
//Set the launcher window state
void LWindow::SetState(LWindowState state)
{
	this->state = state;
}
//Set the launcher windows login data
void LWindow::SetData(LoginData loginData)
{
	this->loginData = loginData;
}
//Set the launcher windows room data
void LWindow::SetData(RoomData roomData)
{
	this->roomData = roomData;
}
//Set received data
void LWindow::SetRecievedData(std::string data)
{
	this->currentData = data;
}
//Set clients name
void LWindow::SetName(std::string name)
{
	this->name = name;
}
//Set next name
void LWindow::SetNextName(std::string name)
{
	this->nextName = name;
}


//Get launcher main window handle
HWND LWindow::GetHandle()
{
	return this->handle;
}
//Get display of a display ID
Display* LWindow::GetDisplay(ui32 displayID)
{
	return &g_launcherDisplays[displayID];
}
//Get launcher windows state
LWindow::LWindowState LWindow::GetState()
{
	return this->state;
}

//Get Login Data
LoginData LWindow::GetLoginData()
{
	return this->loginData;
}
//Get Room Data
RoomData LWindow::GetRoomData()
{
	return this->roomData;
}

//Get received data
std::string LWindow::GetRecievedData()
{
	return this->currentData;
}
//Get name
std::string LWindow::GetName()
{
	return this->name;
}
//Get next name
std::string LWindow::GetNextName()
{
	return this->nextName;
}
//Get reference to the deleted room name
std::string& LWindow::GetDeleteRoom()
{
	return this->deleteRoom;
}

//Get login request state
bool& LWindow::GetQueryState()
{
	return this->queryLogin;
}
//Get state if login was succesful and room frame window should be created
bool& LWindow::GetRoomState()
{
	return this->roomActive;

}
//Get ready state of client
bool& LWindow::GetReadyState()
{
	return this->ready;
}
//Get if client has created a room.
bool& LWindow::GetCreatedState()
{
	return this->created;
}
//Get if data is received to show.
bool& LWindow::GetRecievedState()
{
	return this->recieve;
}
//Get if ready message should be send.
bool& LWindow::GetSendReadyMessage()
{
	return this->sendReady;
}
//Get if data should be send
bool& LWindow::GetSendState()
{
	return this->send;
}