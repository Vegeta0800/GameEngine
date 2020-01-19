
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "filesystem/ra_filesystem.h"
#include "console/ra_console.h"
#include "ra_utils.h"

//Construct the filesystem
Filesystem::Filesystem()
{
	//Get the path where the game is stored
	std::string temp = Console::GetInstancePtr()->GetCVar("gamepath").cvarValue;

	//Set that to the root to the directory
	this->startDirectory = temp.substr(0, temp.find_last_of("/") - 3);

	//List all directories in that directory and store them
	ListDirectories(this->startDirectory.c_str(), this->directories);
}


//List all directories in from a path and recursivly list all others of those aswell
void Filesystem::ListDirectories(const char* startPath, std::vector<std::string>& temp)
{
	//Initilization
	char path[MAX_PATH] = "\0";
	char file[MAX_PATH] = "\0";
	WIN32_FIND_DATA data;
	HANDLE handle;

	//set path to startPath
	sprintf_s(path, "%s*", startPath);

	//Add the path
	temp.push_back(path);

	//Check if there are files in the directory
	if ((handle = FindFirstFile(path, &data)) == INVALID_HANDLE_VALUE)
	{
		LOG("Nothing found in this directory!");
		return;
	}
	
	//List all files in the directory and keep on going until all are listed
	while (FindNextFile(handle, &data))
	{
		//If relevant directory is found
		if ((strncmp("bin", data.cFileName, 3) == 0) 
			|| (strncmp("models", data.cFileName, 6) == 0) 
			|| (strncmp("textures", data.cFileName, 8) == 0)
			|| (strncmp("data", data.cFileName, 4) == 0)
			|| (strncmp("shader", data.cFileName, 6) == 0))
		{
			//If directory
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//Set new path
				sprintf_s(path, "%s%s/", startPath, data.cFileName);
				//Recursion
				ListDirectories(path, temp);
			}
			//If file
			else 
			{
				//Set new file
				sprintf_s(file, "%s%s", startPath, data.cFileName);
				this->files.push_back(file);
			}
		}
 	}
}
//Cleanup
void Filesystem::Cleanup()
{
	delete this;
}


//List all files in a directory
std::vector<std::string> Filesystem::FilesInDirectory(const char* directoryName)
{
	std::vector<std::string> temp;

	//Loop through all files and push back all entries with the same directory
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

	//Return result
	return temp;
}

//Get directory path from name
std::string Filesystem::DirectoryPath(const char* directoryName)
{
	//Trim path to the searched name and return if found
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
//Get name of file in directory
std::string Filesystem::FileInDirectory(const char* directoryName, const char* fileName)
{
	//List all files in the directory
	std::vector<std::string> temp = FilesInDirectory(directoryName);

	//Search for file and if found, return
	for (std::string file : temp)
	{
		std::string dir(file);
		dir = dir.substr(dir.find_last_of("/") + 1);

		if ((strcmp(fileName, dir.c_str()) == 0))
		{
			return file;
		}
	}

	//Return
	return "file wasnt found!";
}

//Get start directory
const char* Filesystem::GetStartDirectory()
{
	return this->startDirectory.c_str();
}