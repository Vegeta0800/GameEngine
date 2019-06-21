
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_types.h"

class Rendering;
class Filesystem;

class Application
{
	DEFINE_SINGLETON(Application)
public:
	void Initialize(const char* path, iVec2 resolution, const char* title);
	void Update();
	void Cleanup();

	Filesystem* GetFilesystem();
	Rendering* GetRenderer();
private:

	Rendering* renderer;
	Filesystem* filesystem;

};