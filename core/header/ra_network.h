
#include <thread>
#include <winsock.h>

class Input;

class Network
{
public:
	void InitializeClient();
	void InitializeHost();

	void Cleanup();
private:
	SOCKET opponentSocket;
	sockaddr_in opponentAddr;

	bool host;
};