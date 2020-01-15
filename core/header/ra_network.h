
#include <thread>
#include <winsock.h>

class Network
{
public:
	void InitializeClient();
	void InitializeHost();

	void Send();

	void Cleanup();

	void SetAddr(sockaddr_in addr);

private:
	SOCKET opponentSocket;
	sockaddr_in opponentAddr;

	std::thread recieveData;

	bool host;
};