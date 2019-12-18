
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_fileinterface.h"
#include "ra_types.h"

class WinFile : public IFile
{
//Declare public functions
public:
	WinFile(const char* path);

	virtual std::string GetName() override;
	virtual std::string GetDirectory() override;
	virtual i64 GetSize() override;
	virtual bool FindFile(const char* path) override;
	virtual byte* Read() override;
	virtual void Cleanup() override;
private:
	virtual void Initialize(const char* path) override;
	std::string mFileName;
	std::string mDirectory;
	byte* mBuffer;
	iptr mHandle;
	ui64 mSize;
};
