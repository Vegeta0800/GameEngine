
#pragma once
//EXTERNAL INCLUDES
#include <vector>
#include <string>
//INTERNAL INCLUDES
#include "ra_types.h"
#include "ra_winfile.h"
#include "ra_utils.h"

class Filesystem
{
public:
	Filesystem();

	void ListDirectories(const char* startPath, std::vector<std::string>& temp);
	std::string DirectoryPath(const char* directoryName);
	std::string FileInDirectory(const char* directoryName, const char* fileName);
	std::vector<std::string> FilesInDirectory(const char* directoryName);
	const char* GetStartDirectory();
	void Cleanup();
private:
	std::vector<std::string> directories;
	std::vector<std::string> files;

	std::string startDirectory;
};