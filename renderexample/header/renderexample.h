
#pragma once
// EXTERNAL INCLUDES
#include <thread>
#include <vector>
#include <list>
// INTERNAL INCLUDES
#include "ra_rendering.h"
#include "ra_gameobject.h"
#include "ra_types.h"
#include "ra_utils.h"
#include "console/ra_console.h"
#include "filesystem/ra_filesystem.h"

class RenderExample
{
	//Define Application as a singleton. See utils.h for definition.
	DEFINE_SINGLETON(RenderExample)

		//Declare public functions.
public:
	void Initialize(char* title, iVec2 resolution, ui32 displayID = 0);
	void Update(void);
	void CleanUp(void);

	Filesystem* GetFilesystem();
	Rendering* GetRenderer();

	//Declare private variables
private:

	const int FRAMES_PER_SECOND = 144;
	const int SKIP_TICKS = 1000 / FRAMES_PER_SECOND;
	bool running = true;

	Filesystem* filesystem;
	Rendering* renderer;
	Gameobject* root;

	std::vector<Gameobject*> gameObjects;
};