
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

	void SetOpponent(SOCKADDR_IN addr);

	Filesystem* GetFilesystem();
	float GetDeltaTime();
	float& GetAspectRatio();

	bool& GetEstablishState();
	bool& GetStartUpState();
	SOCKADDR_IN GetOpponent();
private:
	Filesystem* filesystem;

	SOCKADDR_IN opponent;

	float deltaTime = 0.0f;
	float aspectRatio = 0.0f;

	bool startUp;
	bool establishConnection;
};