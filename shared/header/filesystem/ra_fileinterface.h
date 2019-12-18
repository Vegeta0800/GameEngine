
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_types.h"

class IFile
{
	//Declare public functions

public:
	virtual std::string GetName() = 0;
	virtual std::string  GetDirectory() = 0;
	virtual i64 GetSize() = 0;
	virtual bool FindFile(const char* path) = 0;
	virtual byte* Read() = 0;
	virtual void Cleanup() = 0;

private:
	virtual void Initialize(const char* path) = 0;

};
