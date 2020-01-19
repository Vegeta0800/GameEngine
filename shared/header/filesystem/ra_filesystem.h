
#pragma once
//EXTERNAL INCLUDES
#include <vector>
#include <string>
//INTERNAL INCLUDES
#include "ra_types.h"
#include "ra_winfile.h"
#include "ra_utils.h"

//Filesystem class to store, load and lookup files
class Filesystem
{
public:
	//Construct the filesystem
	Filesystem();


	//List all directories in from a path and recursivly list all others of those aswell
	void ListDirectories(const char* startPath, std::vector<std::string>& temp);
	//Cleanup
	void Cleanup();


	//List all files in a directory
	std::vector<std::string> FilesInDirectory(const char* directoryName);
	
	//Get directory path from name
	std::string DirectoryPath(const char* directoryName);
	//Get name of file in directory
	std::string FileInDirectory(const char* directoryName, const char* fileName);
	
	//Get start directory
	const char* GetStartDirectory();

private:
	std::vector<std::string> directories;
	std::vector<std::string> files;

	std::string startDirectory;
};