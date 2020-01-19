
//EXTERNAL INCLUDES
#include <string>
//INTERNAL INCLUDES
#include "filesystem/ra_winfile.h"
#include "ra_utils.h"

//Construct windows file with a path to a file
WinFile::WinFile(const char* path)
{
	Initialize(path);
}


//Initialize a windows file
void WinFile::Initialize(const char* path)
{
	//handle to file
	HANDLE hfile;

	//Try to find the file and open it. If it doesnt exist create it.
	if (!FindFile(path))
	{
		LOG("Failed to read file.")
		throw;
	}

	//Create file handle by opening file at path
	hfile = CreateFileA
	(
		path,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	//Get Data
	LARGE_INTEGER fileSizeHigh = { 0 };
	GetFileSizeEx(hfile, &fileSizeHigh);
	byte* buffer = reinterpret_cast<byte*>(malloc(fileSizeHigh.QuadPart));
	memset(buffer, 0, fileSizeHigh.QuadPart);
	DWORD readBytes = 0;

	//Read the file and save the file into the buffer. If fails crash the program
	if (!ReadFile(hfile, buffer, static_cast<DWORD>(fileSizeHigh.QuadPart), &readBytes, NULL))
	{
		LOG("Failed to read file.")
		throw;
	}

	//Get file directory
	std::string dir(path);
	dir = dir.substr(0, dir.find_last_of("/"));
	dir = dir.substr(dir.find_last_of("/") + 1);

	//Get file name
	std::string name(path);
	name = name.substr(name.find_last_of("/") + 1);

	//Set variables
	this->mFileName = name;
	this->mDirectory = dir;
	this->mHandle = reinterpret_cast<iptr>(hfile);
	this->mBuffer = buffer;
	this->mSize = fileSizeHigh.QuadPart;
}
//Cleanup file
void WinFile::Cleanup()
{
	CloseHandle(reinterpret_cast<HANDLE>(this->mHandle));
	this->mHandle = 0;
	this->mBuffer = nullptr;
}

//Is a file present at path
bool WinFile::FindFile(const char* path)
{
	WIN32_FIND_DATA findData = { 0 };

	//Try to find the file. And return the result.
	if(FindFirstFile(path, &findData) == INVALID_HANDLE_VALUE)
		return false;
	return true;
}

//Get byte buffer of file
byte* WinFile::Read()
{
	return this->mBuffer;
}


//Get name of file
std::string WinFile::GetName()
{
	return this->mFileName;
}
//Get name of directory
std::string WinFile::GetDirectory()
{
	return this->mDirectory;
}

//Get file size
i64 WinFile::GetSize()
{
	return this->mSize;
}