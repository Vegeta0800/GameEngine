#include <string>

#include "filesystem/ra_winfile.h"
#include "ra_utils.h"

WinFile::WinFile(const char* path)
{
	Initialize(path);
}

void WinFile::Initialize(const char* path)
{
	HANDLE hfile;

	//Try to find the file and open it. If it doesnt exist create it.
	if (!FindFile(path))
	{
		LOG("Failed to read file.")
		throw;
	}

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

	std::string dir(path);
	dir = dir.substr(0, dir.find_last_of("/"));
	dir = dir.substr(dir.find_last_of("/") + 1);

	std::string name(path);
	name = name.substr(name.find_last_of("/") + 1);

	//Set variables
	this->mFileName = name;
	this->mDirectory = dir;
	this->mHandle = reinterpret_cast<iptr>(hfile);
	this->mBuffer = buffer;
	this->mSize = fileSizeHigh.QuadPart;
}

bool WinFile::FindFile(const char* path)
{
	WIN32_FIND_DATA findData = { 0 };

	//Try to find the file. And return the result.
	if(FindFirstFile(path, &findData) == INVALID_HANDLE_VALUE)
		return false;
	return true;
}

std::string WinFile::GetName()
{
	return this->mFileName;
}

std::string WinFile::GetDirectory()
{
	return this->mDirectory;
}

i64 WinFile::GetSize()
{
	return this->mSize;
}

byte* WinFile::Read()
{
	return this->mBuffer;
}

void WinFile::Cleanup()
{
	CloseHandle(reinterpret_cast<HANDLE>(this->mHandle));
	this->mHandle = 0;
	this->mBuffer = nullptr;
}



