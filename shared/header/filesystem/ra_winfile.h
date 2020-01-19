
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_fileinterface.h"
#include "ra_types.h"

//Windows file implementation
class WinFile : public IFile
{
//Declare public functions
public:
	//Construct windows file with a path to a file
	WinFile(const char* path);


	//Initialize a windows file
	virtual void Initialize(const char* path) override;
	//Cleanup file
	virtual void Cleanup() override;

	//Is a file present at path
	virtual bool FindFile(const char* path) override;

	//Get byte buffer of file
	virtual byte* Read() override;
		
	
	//Get name of file
	virtual std::string GetName() override;
	//Get name of directory
	virtual std::string GetDirectory() override;

	//Get file size
	virtual i64 GetSize() override;

private:

	std::string mFileName;
	std::string mDirectory;

	byte* mBuffer;
	iptr mHandle;

	ui64 mSize;
};
