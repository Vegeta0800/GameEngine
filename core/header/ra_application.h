
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_types.h"
#include <thread>
#include <string>

class Filesystem;
	
class Application
{
	DEFINE_SINGLETON(Application)
public:
	void Initialize(const char* path, iVec2 resolution, const char* title);

	void Update();
	void Cleanup();

	void SetOpponent(std::string addr);

	Filesystem* GetFilesystem();
	float GetDeltaTime();
	float& GetAspectRatio();

	bool& GetEstablishState();
	bool& GetHostState();
	bool& GetRunState();

	std::string GetOpponent();
private:
	Filesystem* filesystem;
	
	std::thread networkThread;

	std::string opponent;

	float deltaTime = 0.0f;
	float aspectRatio = 0.0f;

	bool host;
	bool running;
	bool establishConnection;
};