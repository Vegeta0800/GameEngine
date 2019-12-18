
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_types.h"

class Filesystem;
	
class Application
{
	DEFINE_SINGLETON(Application)
public:
	void Initialize(const char* path, iVec2 resolution, const char* title);

	void Update();
	void Cleanup();

	Filesystem* GetFilesystem();
	float GetDeltaTime();
	float& GetAspectRatio();

	bool& GetEstablishState();
	bool& GetStartUpState();
private:
	Filesystem* filesystem;

	float deltaTime = 0.0f;
	float aspectRatio = 0.0f;

	bool startUp;
	bool establishConnection;
};