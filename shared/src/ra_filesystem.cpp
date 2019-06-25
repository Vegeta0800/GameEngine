#include "filesystem/ra_filesystem.h"
#include "ra_utils.h"
#include "console/ra_console.h"

Filesystem::Filesystem()
{

	ListDirectories(Console::GetInstancePtr()->GetCVar("gamepath").cvarValue, this->directories);

	std::string temp = Console::GetInstancePtr()->GetCVar("gamepath").cvarValue;
	this->startDirectory = temp.substr(0, temp.find_last_of("/") - 4);
}

void Filesystem::ListDirectories(const char* startPath, std::vector<std::string>& temp)
{
	char path[MAX_PATH] = "\0";
	char file[MAX_PATH] = "\0";
	WIN32_FIND_DATA data;
	HANDLE handle;

	sprintf_s(path, "%s*", startPath);

	temp.push_back(path);

	if ((handle = FindFirstFile(path, &data)) == INVALID_HANDLE_VALUE) 
	{
		LOG("Nothing found in this directory!");
		return;
	}
	
	while (FindNextFile(handle, &data))
	{
		if ((strncmp(".", data.cFileName, 1) != 0) && (strncmp("..", data.cFileName, 2) != 0) && (strncmp("includes", data.cFileName, 8) != 0) && (strncmp("src", data.cFileName, 3) != 0))
		{
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sprintf_s(path, "%s%s/", startPath, data.cFileName);
				ListDirectories(path, temp);
			}
			else 
			{
				sprintf_s(file, "%s%s", startPath, data.cFileName);
				this->files.push_back(file);
			}
		}
 	}
}

std::string Filesystem::DirectoryPath(const char* directoryName)
{
	for (std::string path : this->directories)
	{
		std::string temp(path);

		temp = temp.substr(0, temp.find_last_of("/"));
		temp = temp.substr(temp.find_last_of("/") + 1);

		if ((strcmp(directoryName, temp.c_str()) == 0))
		{
			return path.c_str();
		}
	}

	return nullptr;
}

std::string Filesystem::FileInDirectory(const char* directoryName, const char* fileName)
{
	std::vector<std::string> temp = FilesInDirectory(directoryName);

	for (std::string file : temp)
	{
		std::string dir(file);
		dir = dir.substr(dir.find_last_of("/") + 1);

		if ((strcmp(fileName, dir.c_str()) == 0))
		{
			return file;
		}
	}

	return "file wasnt found!";
}

const char* Filesystem::GetStartDirectory()
{
	return this->startDirectory.c_str();
}

std::vector<std::string> Filesystem::FilesInDirectory(const char* directoryName)
{
	std::vector<std::string> temp;

	for (std::string file : this->files)
	{
		std::string dir(file);

		dir = dir.substr(0, dir.find_last_of("/"));
		dir = dir.substr(dir.find_last_of("/") + 1);

		if ((strcmp(directoryName, dir.c_str()) == 0))
		{
			temp.push_back(file);
		}
	}

	return temp;
}

void Filesystem::Cleanup()
{
	delete this;
}


